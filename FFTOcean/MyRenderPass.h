#pragma once

#include<Gear/Core/RenderPass.h>

#include<Gear/Core/Shader.h>

#include<Gear/Core/Resource/TextureRenderView.h>

#include<Gear/Core/Effect/BloomEffect.h>

#include<DirectXColors.h>

class MyRenderPass :public RenderPass
{
public:

	MyRenderPass() :
		spectrumParamBuffer(ResourceManager::createConstantBuffer(sizeof(SpectrumParam), true)),
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

		tildeh0Texture = createTexture(textureResolution + 1, DXGI_FORMAT_R32G32_FLOAT);

		waveDataTexture = createTexture(textureResolution + 1, DXGI_FORMAT_R32G32B32A32_FLOAT);

		waveSpectrumTexture = createTexture(textureResolution, DXGI_FORMAT_R32G32B32A32_FLOAT);

		tempTexture = createTexture(textureResolution, DXGI_FORMAT_R32G32_FLOAT);

		DxDz = createTexture(textureResolution, DXGI_FORMAT_R32G32_FLOAT);

		DyDxz = createTexture(textureResolution, DXGI_FORMAT_R32G32_FLOAT);

		DyxDyz = createTexture(textureResolution, DXGI_FORMAT_R32G32_FLOAT);

		DxxDzz = createTexture(textureResolution, DXGI_FORMAT_R32G32_FLOAT);

		displacementTexture = createTexture(textureResolution, DXGI_FORMAT_R32G32B32A32_FLOAT);

		derivativeTexture = createTexture(textureResolution, DXGI_FORMAT_R32G32B32A32_FLOAT);

		jacobianTexture = createTexture(textureResolution, DXGI_FORMAT_R32_FLOAT);

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
					const float xPos = (float)x * (float)spectrumParam.mapLength / (float)tilePerPatch - (float)patchNum * spectrumParam.mapLength / 2.f;

					const float zPos = (float)z * (float)spectrumParam.mapLength / (float)tilePerPatch - (float)patchNum * spectrumParam.mapLength;

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

		randomGaussTexture = resManager->createTextureRenderView(textureResolution + 1, textureResolution + 1, RandomDataType::GAUSS, true);

		enviromentCube = resManager->createTextureCube("E:\\Assets\\Ocean\\ColdSunsetEquirect.png", 1024, true);

		calculateInitialSpectrum();

		{
			const float clearValue[] = { 999.f,999.f,999.f,999.f };

			context->clearUnorderedAccess(jacobianTexture->getClearUAVDesc(0), clearValue);
		}

		end();

		RenderEngine::get()->submitRenderPass(this);

		randomGaussTexture->getTexture()->setName(L"randomGaussTexture");

		tildeh0Texture->getTexture()->setName(L"tildeh0Texture");

		waveDataTexture->getTexture()->setName(L"waveDataTexture");

		waveSpectrumTexture->getTexture()->setName(L"waveSpectrumTexture");

		DxDz->getTexture()->setName(L"DxDz");

		DyDxz->getTexture()->setName(L"DyDxz");

		DyxDyz->getTexture()->setName(L"DyxDyz");

		DxxDzz->getTexture()->setName(L"DxxDzz");

		displacementTexture->getTexture()->setName(L"displacementTexture");

		derivativeTexture->getTexture()->setName(L"derivativeTexture");

		jacobianTexture->getTexture()->setName(L"jacobianTexture");

		originTexture->getTexture()->setName(L"originTexture");

		depthTexture->getTexture()->setName(L"depthTexture");

		oceanState->SetName(L"oceanState");

		effect->setExposure(1.5f);

		effect->setGamma(0.6f);
	}

	~MyRenderPass()
	{
		delete vertexBuffer;

		delete spectrumParamBuffer;

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

		delete waveDataTexture;

		delete waveSpectrumTexture;

		delete tempTexture;

		delete DxDz;

		delete DyDxz;

		delete DyxDyz;

		delete DxxDzz;

		delete displacementTexture;

		delete derivativeTexture;

		delete jacobianTexture;

		delete originTexture;

		delete depthTexture;

		delete effect;
	}

	void imGUICall() override
	{
		effect->imGUICall();
	}

protected:

	void recordCommand() override
	{
		calculateTimeDependentSpectrum();

		calculateDisplacementAndDerivative();

		renderSkyDomeAndOceanSurface();
	}

private:

	//do not change this!
	static constexpr UINT textureResolution = 1024;

	static constexpr UINT patchNum = 5;

	static constexpr UINT tilePerPatch = 8;

	static TextureRenderView* createTexture(const UINT& resolution, const DXGI_FORMAT& format)
	{
		return ResourceManager::createTextureRenderView(resolution, resolution, format, 1, 1, false, true, format, format, DXGI_FORMAT_UNKNOWN);
	}

	void ifftPermutation(TextureRenderView* const inputTexture)
	{
		context->setPipelineState(ifftState.Get());

		context->setCSConstants({
			tempTexture->getUAVMipIndex(0),
			inputTexture->getAllSRVIndex() }, 0);

		context->transitionResources();

		context->dispatch(textureResolution, 1, 1);

		context->uavBarrier({
			tempTexture->getTexture() });

		context->setCSConstants({
			inputTexture->getUAVMipIndex(0),
			tempTexture->getAllSRVIndex() }, 0);

		context->transitionResources();

		context->dispatch(textureResolution, 1, 1);

		context->uavBarrier({
			inputTexture->getTexture() });

		context->setPipelineState(permutationState.Get());

		context->setCSConstants({
			inputTexture->getUAVMipIndex(0) }, 0);

		context->transitionResources();

		context->dispatch(textureResolution / 8, textureResolution / 8, 1);

		context->uavBarrier({
			inputTexture->getTexture() });
	}

	void calculateInitialSpectrum()
	{
		spectrumParamBuffer->update(&spectrumParam, sizeof(SpectrumParam));

		context->setPipelineState(spectrumState.Get());

		context->setCSConstants({
			tildeh0Texture->getUAVMipIndex(0),
			waveDataTexture->getUAVMipIndex(0),
			randomGaussTexture->getAllSRVIndex() }, 0);

		context->setCSConstantBuffer(spectrumParamBuffer);

		context->transitionResources();

		context->dispatch(textureResolution / 8 + 1, textureResolution / 8 + 1, 1);

		context->uavBarrier({
			tildeh0Texture->getTexture(),
			waveDataTexture->getTexture() });

		context->setPipelineState(conjugateState.Get());

		context->setCSConstants({
			waveSpectrumTexture->getUAVMipIndex(0),
			tildeh0Texture->getAllSRVIndex() }, 0);

		context->transitionResources();

		context->dispatch(textureResolution / 8, textureResolution / 8, 1);

		context->uavBarrier({
			waveSpectrumTexture->getTexture() });
	}

	void calculateTimeDependentSpectrum()
	{
		context->setPipelineState(displacementSpectrumState.Get());

		context->setCSConstants({
			DxDz->getUAVMipIndex(0),
			DyDxz->getUAVMipIndex(0),
			DyxDyz->getUAVMipIndex(0),
			DxxDzz->getUAVMipIndex(0),
			waveDataTexture->getAllSRVIndex(),
			waveSpectrumTexture->getAllSRVIndex() }, 0);

		context->transitionResources();

		context->dispatch(textureResolution / 8, textureResolution / 8, 1);

		context->uavBarrier({
			DxDz->getTexture(),
			DyDxz->getTexture(),
			DyxDyz->getTexture(),
			DxxDzz->getTexture() });
	}

	void calculateDisplacementAndDerivative()
	{
		ifftPermutation(DxDz);

		ifftPermutation(DyDxz);

		ifftPermutation(DyxDyz);

		ifftPermutation(DxxDzz);

		context->setPipelineState(waveMergeState.Get());

		context->setCSConstants({
			displacementTexture->getUAVMipIndex(0),
			derivativeTexture->getUAVMipIndex(0),
			jacobianTexture->getUAVMipIndex(0),
			DxDz->getAllSRVIndex(),
			DyDxz->getAllSRVIndex(),
			DyxDyz->getAllSRVIndex(),
			DxxDzz->getAllSRVIndex() }, 0);

		context->transitionResources();

		context->dispatch(textureResolution / 8, textureResolution / 8, 1);

		context->uavBarrier({
			displacementTexture->getTexture(),
			derivativeTexture->getTexture(),
			jacobianTexture->getTexture() });
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
			displacementTexture->getAllSRVIndex(),
			derivativeTexture->getAllSRVIndex(),
			jacobianTexture->getAllSRVIndex() }, 0);

		context->setPSConstants({
			displacementTexture->getAllSRVIndex(),
			derivativeTexture->getAllSRVIndex(),
			jacobianTexture->getAllSRVIndex(),
			enviromentCube->getAllSRVIndex() }, 0);

		context->transitionResources();

		context->clearDepthStencil(depthTexture->getDSVMipHandle(0), D3D12_CLEAR_FLAG_DEPTH, 1.f, 0);

		context->draw(4 * (patchNum * patchNum) * (tilePerPatch * tilePerPatch), 1, 0, 0);

		TextureRenderView* bloomTexture = effect->process(originTexture);

		blit(bloomTexture);
	}

	struct SpectrumParam
	{
		UINT mapResolution = textureResolution;
		float mapLength = 512.f;
		DirectX::XMFLOAT2 wind = { 12.5f,0.f };
		float amplitude = 0.0000005f;
		float gravity = 9.81f;
		DirectX::XMFLOAT2 padding0;
		DirectX::XMFLOAT4 padding1[14];
	} spectrumParam;

	BufferView* vertexBuffer;

	ConstantBuffer* spectrumParamBuffer;

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

	//k.x 1.0/length(K) k.z angularSpeed
	//x y z w
	TextureRenderView* waveDataTexture;

	//(tildeh0(k), conj(tildeh0(-k)))
	//x y z w
	TextureRenderView* waveSpectrumTexture;

	//intermediate texture for ifft compute
	TextureRenderView* tempTexture;

	//following 4 textures store displacement and derivative(can be further calculated to normal and jacobian)

	//Dx Dz
	TextureRenderView* DxDz;

	//Dy dDx/dz
	TextureRenderView* DyDxz;

	//dDy/dx dDy/dz
	TextureRenderView* DyxDyz;

	//dDx/dx dDz/dz
	TextureRenderView* DxxDzz;

	//Dx Dy Dz
	TextureRenderView* displacementTexture;

	//dDy/dx dDy/dz dDx/dx dDz/dz
	TextureRenderView* derivativeTexture;

	//J
	TextureRenderView* jacobianTexture;

	TextureRenderView* originTexture;

	TextureDepthView* depthTexture;

	BloomEffect* effect;

};