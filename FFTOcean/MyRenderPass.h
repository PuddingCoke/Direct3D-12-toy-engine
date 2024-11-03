#pragma once

#include<Gear/Core/RenderPass.h>

#include<Gear/Core/Shader.h>

#include<Gear/Core/Resource/TextureRenderView.h>

#include<Gear/Core/Effect/BloomEffect.h>

#include<DirectXColors.h>

#include"WaveCascade.h"

class MyRenderPass :public RenderPass
{
public:

	MyRenderPass() :
		spectrumParamBuffer{ ResourceManager::createConstantBuffer(sizeof(SpectrumParam), true),ResourceManager::createConstantBuffer(sizeof(SpectrumParam), true),ResourceManager::createConstantBuffer(sizeof(SpectrumParam), true) },
		cascade{ new WaveCascade(textureResolution,context),new WaveCascade(textureResolution,context),new WaveCascade(textureResolution,context) },
		textureCubePS(new Shader(Utils::getRootFolder() + "TextureCubePS.cso")),
		oceanVS(new Shader(Utils::getRootFolder() + "OceanVS.cso")),
		oceanHS(new Shader(Utils::getRootFolder() + "OceanHS.cso")),
		oceanDS(new Shader(Utils::getRootFolder() + "OceanDS.cso")),
		oceanPS(new Shader(Utils::getRootFolder() + "OceanPS.cso")),
		spectrumCS(new Shader(Utils::getRootFolder() + "SpectrumCS.cso")),
		conjugateCS(new Shader(Utils::getRootFolder() + "ConjugateCS.cso")),
		displacementSpectrumCS(new Shader(Utils::getRootFolder() + "DisplacementSpectrumCS.cso")),
		ifftCS(new Shader(Utils::getRootFolder() + "IFFTCS.cso")),
		permutationCS(new Shader(Utils::getRootFolder() + "PermutationCS.cso")),
		waveMergeCS(new Shader(Utils::getRootFolder() + "WaveMergeCS.cso"))
	{
		spectrumParam[0].mapLength = lengthScale0;

		spectrumParam[1].mapLength = lengthScale1;

		spectrumParam[2].mapLength = lengthScale2;

		{
			D3D12_GRAPHICS_PIPELINE_STATE_DESC desc = PipelineState::getDefaultGraphicsDesc();
			desc.InputLayout = {};
			desc.pRootSignature = GlobalRootSignature::getGraphicsRootSignature()->get();
			desc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
			desc.RasterizerState = States::rasterCullBack;
			desc.DepthStencilState.DepthEnable = false;
			desc.DepthStencilState.StencilEnable = false;
			desc.SampleMask = UINT_MAX;
			desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
			desc.NumRenderTargets = 1;
			desc.RTVFormats[0] = DXGI_FORMAT_R16G16B16A16_FLOAT;
			desc.SampleDesc.Count = 1;
			desc.VS = Shader::textureCubeVS->getByteCode();
			desc.PS = textureCubePS->getByteCode();

			GraphicsDevice::get()->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(&textureCubeState));
		}

		{
			D3D12_INPUT_ELEMENT_DESC inputLayoutDesc[] =
			{
				{"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},
				{"TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0}
			};

			D3D12_GRAPHICS_PIPELINE_STATE_DESC desc = PipelineState::getDefaultGraphicsDesc();
			desc.InputLayout = { inputLayoutDesc,_countof(inputLayoutDesc) };
			desc.pRootSignature = GlobalRootSignature::getGraphicsRootSignature()->get();
			desc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
			desc.RasterizerState = States::rasterCullBack;
			desc.DepthStencilState = States::depthLess;
			desc.SampleMask = UINT_MAX;
			desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_PATCH;
			desc.NumRenderTargets = 1;
			desc.RTVFormats[0] = DXGI_FORMAT_R16G16B16A16_FLOAT;
			desc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
			desc.SampleDesc.Count = 1;
			desc.VS = oceanVS->getByteCode();
			desc.HS = oceanHS->getByteCode();
			desc.DS = oceanDS->getByteCode();
			desc.PS = oceanPS->getByteCode();

			GraphicsDevice::get()->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(&oceanState));
		}

		PipelineState::createComputeState(&spectrumState, spectrumCS);

		PipelineState::createComputeState(&conjugateState, conjugateCS);

		PipelineState::createComputeState(&displacementSpectrumState, displacementSpectrumCS);

		PipelineState::createComputeState(&ifftState, ifftCS);

		PipelineState::createComputeState(&permutationState, permutationCS);

		PipelineState::createComputeState(&waveMergeState, waveMergeCS);

		tildeh0Texture = createTexture(textureResolution, DXGI_FORMAT_R32G32_FLOAT);

		tempTexture = createTexture(textureResolution, DXGI_FORMAT_R32G32_FLOAT);

		WaveCascade::spectrumState = spectrumState.Get();

		WaveCascade::conjugateState = conjugateState.Get();

		WaveCascade::displacementSpectrumState = displacementSpectrumState.Get();

		WaveCascade::ifftState = ifftState.Get();

		WaveCascade::permutationState = permutationState.Get();

		WaveCascade::waveMergeState = waveMergeState.Get();

		WaveCascade::tildeh0Texture = tildeh0Texture;

		WaveCascade::tempTexture = tempTexture;

		originTexture = ResourceManager::createTextureRenderView(Graphics::getWidth(), Graphics::getHeight(), DXGI_FORMAT_R16G16B16A16_FLOAT, 1, 1, false, true,
			DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_R16G16B16A16_FLOAT, DirectX::Colors::Black);

		depthTexture = ResourceManager::createTextureDepthView(Graphics::getWidth(), Graphics::getHeight(), DXGI_FORMAT_R32_TYPELESS, 1, 1, false, true);

		struct Vertex
		{
			DirectX::XMFLOAT3 position;
			DirectX::XMFLOAT2 uv;
		};

		std::vector<Vertex> vertices;

		{
			auto getVertexAt = [this](const UINT& x, const UINT& z)
				{
					const float xPos = (float)x * (float)spectrumParam[0].mapLength / (float)tilePerPatch - (float)patchNum * spectrumParam[0].mapLength / 2.f;

					const float zPos = (float)z * (float)spectrumParam[0].mapLength / (float)tilePerPatch - (float)patchNum * spectrumParam[0].mapLength;

					const float u = (float)x / (float)tilePerPatch;

					const float v = (float)z / (float)tilePerPatch;

					return Vertex{ {xPos,0.f,zPos},{u,v} };
				};

			for (UINT patchZ = 0; patchZ < patchNum; patchZ++)
			{
				for (UINT patchX = 0; patchX < patchNum; patchX++)
				{
					for (UINT tileZ = 0; tileZ < tilePerPatch; tileZ++)
					{
						for (UINT tileX = 0; tileX < tilePerPatch; tileX++)
						{
							const UINT z = patchZ * tilePerPatch + tileZ;

							const UINT x = patchX * tilePerPatch + tileX;

							vertices.push_back(getVertexAt(x, z));

							vertices.push_back(getVertexAt(x, z + 1));

							vertices.push_back(getVertexAt(x + 1, z + 1));

							vertices.push_back(getVertexAt(x + 1, z));
						}
					}
				}
			}
		}

		begin();

		effect = new BloomEffect(context, Graphics::getWidth(), Graphics::getHeight(), resManager);

		vertexBuffer = resManager->createStructuredBufferView(sizeof(Vertex), static_cast<UINT>(sizeof(Vertex) * vertices.size()), false, false, true, false, true, vertices.data());

		randomGaussTexture = resManager->createTextureRenderView(textureResolution, textureResolution, RandomDataType::GAUSS, true);

		WaveCascade::randomGaussTexture = randomGaussTexture;

		enviromentCube = resManager->createTextureCube("E:\\Assets\\Ocean\\ColdSunsetEquirect.png", 2048, true);

		calculateInitialSpectrum();

		end();

		RenderEngine::get()->submitRenderPass(this);

		tempTexture->getTexture()->setName(L"tempTexture");

		randomGaussTexture->getTexture()->setName(L"randomGaussTexture");

		tildeh0Texture->getTexture()->setName(L"tildeh0Texture");

		originTexture->getTexture()->setName(L"originTexture");

		depthTexture->getTexture()->setName(L"depthTexture");

		oceanState->SetName(L"oceanState");

		effect->setExposure(1.5f);

		effect->setGamma(0.6f);
	}

	~MyRenderPass()
	{
		delete vertexBuffer;

		delete textureCubePS;

		delete oceanVS;

		delete oceanHS;

		delete oceanDS;

		delete oceanPS;

		delete spectrumCS;

		delete conjugateCS;

		delete displacementSpectrumCS;

		delete ifftCS;

		delete permutationCS;

		delete waveMergeCS;

		delete enviromentCube;

		delete randomGaussTexture;

		delete tildeh0Texture;

		delete tempTexture;

		delete originTexture;

		delete depthTexture;

		delete effect;

		for (UINT i = 0; i < cascadeNum; i++)
		{
			delete spectrumParamBuffer[i];

			delete cascade[i];
		}
	}

	void imGUICall() override
	{
		effect->imGUICall();
	}

protected:

	void recordCommand() override
	{
		calculateDisplacementAndDerivative();

		renderSkyDomeAndOceanSurface();
	}

private:

	//do not change this!
	static constexpr UINT cascadeNum = 3;

	static constexpr float lengthScale0 = 500.f;

	static constexpr float lengthScale1 = 27.f;

	static constexpr float lengthScale2 = 5.f;

	//do not change this!
	static constexpr UINT textureResolution = 512;

	static constexpr UINT patchNum = 5;

	static constexpr UINT tilePerPatch = 8;

	static TextureRenderView* createTexture(const UINT& resolution, const DXGI_FORMAT& format)
	{
		return ResourceManager::createTextureRenderView(resolution, resolution, format, 1, 1, false, true, format, format, DXGI_FORMAT_UNKNOWN);
	}

	void calculateInitialSpectrum()
	{
		const float boundary1 = 2.f * Math::pi / lengthScale1 * 7.f;

		const float boundary2 = 2.f * Math::pi / lengthScale2 * 7.f;

		std::cout << "boundary 1 " << boundary1 << "\n";

		std::cout << "boundary 2 " << boundary2 << "\n";

		{
			spectrumParam[0].cutoffLow = 0.0001f;

			spectrumParam[0].cutoffHigh = boundary1;
		}

		{
			spectrumParam[1].cutoffLow = boundary1;

			spectrumParam[1].cutoffHigh = boundary2;
		}

		{
			spectrumParam[2].cutoffLow = boundary2;

			spectrumParam[2].cutoffHigh = 9999.f;
		}

		for (UINT i = 0; i < cascadeNum; i++)
		{
			spectrumParamBuffer[i]->update(&spectrumParam[i], sizeof(SpectrumParam));

			cascade[i]->calculateInitialSpectrum(spectrumParamBuffer[i]);
		}
	}

	void calculateDisplacementAndDerivative()
	{
		for (UINT i = 0; i < cascadeNum; i++)
		{
			cascade[i]->calculateTimeDependentSpectrum();

			cascade[i]->calculateDisplacementAndDerivative();
		}
	}

	void renderSkyDomeAndOceanSurface()
	{
		context->setPipelineState(textureCubeState.Get());

		context->setViewportSimple(Graphics::getWidth(), Graphics::getHeight());

		context->setTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		context->setRenderTargets({ originTexture->getRTVMipHandle(0) }, {});

		context->setPSConstants({
			enviromentCube->getAllSRVIndex() }, 0);

		context->transitionResources();

		context->draw(36, 1, 0, 0);

		context->setPipelineState(oceanState.Get());

		context->setViewportSimple(Graphics::getWidth(), Graphics::getHeight());

		context->setTopology(D3D_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST);

		context->setVertexBuffers(0, { vertexBuffer->getVertexBuffer() });

		const DepthStencilDesc depthStencilDesc = depthTexture->getDSVMipHandle(0);

		context->setRenderTargets({ originTexture->getRTVMipHandle(0) }, &depthStencilDesc);

		context->setDSConstants({
			cascade[0]->displacementTexture->getAllSRVIndex(),
			cascade[0]->derivativeTexture->getAllSRVIndex(),
			cascade[0]->jacobianTexture->getAllSRVIndex() }, 0);

		context->setPSConstants({
			cascade[0]->displacementTexture->getAllSRVIndex(),
			cascade[0]->derivativeTexture->getAllSRVIndex(),
			cascade[0]->jacobianTexture->getAllSRVIndex(),
			enviromentCube->getAllSRVIndex() }, 0);

		context->transitionResources();

		context->clearDepthStencil(depthTexture->getDSVMipHandle(0), D3D12_CLEAR_FLAG_DEPTH, 1.f, 0);

		context->draw(4 * (patchNum * patchNum) * (tilePerPatch * tilePerPatch), 1, 0, 0);

		TextureRenderView* bloomTexture = effect->process(originTexture);

		blit(bloomTexture);
	}

	struct SpectrumParam
	{
		const UINT mapResolution = textureResolution;
		float mapLength;
		const DirectX::XMFLOAT2 wind = { 12.f,0.f };
		const float amplitude = 0.0000015f;
		const float gravity = 9.81f;
		float cutoffLow;
		float cutoffHigh;
		const DirectX::XMFLOAT4 padding1[14] = {};
	} spectrumParam[cascadeNum];

	BufferView* vertexBuffer;

	Shader* textureCubePS;

	ComPtr<ID3D12PipelineState> textureCubeState;

	Shader* oceanVS;

	Shader* oceanHS;

	Shader* oceanDS;

	Shader* oceanPS;

	ComPtr<ID3D12PipelineState> oceanState;

	Shader* spectrumCS;

	ComPtr<ID3D12PipelineState> spectrumState;

	Shader* conjugateCS;

	ComPtr<ID3D12PipelineState> conjugateState;

	Shader* displacementSpectrumCS;

	ComPtr<ID3D12PipelineState> displacementSpectrumState;

	Shader* ifftCS;

	ComPtr<ID3D12PipelineState> ifftState;

	//sign correction
	Shader* permutationCS;

	ComPtr<ID3D12PipelineState> permutationState;

	Shader* waveMergeCS;

	ComPtr<ID3D12PipelineState> waveMergeState;

	TextureRenderView* enviromentCube;

	//4 channel random gaussian distribution texture
	//mean 0 standard deviation 1
	TextureRenderView* randomGaussTexture;

	//(tildeh0(k))
	//x y
	TextureRenderView* tildeh0Texture;

	//intermediate texture for ifft compute
	TextureRenderView* tempTexture;

	TextureRenderView* originTexture;

	TextureDepthView* depthTexture;

	BloomEffect* effect;

	ConstantBuffer* spectrumParamBuffer[cascadeNum];

	WaveCascade* cascade[cascadeNum];

};