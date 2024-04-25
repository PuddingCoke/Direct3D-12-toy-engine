#pragma once

#include<Gear/Core/RenderPass.h>

#include<Gear/Core/Shader.h>

#include<Gear/Core/Effect/BloomEffect.h>

class MyRenderPass :public RenderPass
{
public:

	MyRenderPass() :
		oceanParamBuffer(ResourceManager::createConstantBuffer(sizeof(Param), true)),
		textureCubePS(new Shader(Utils::getRootFolder() + "TextureCubePS.cso")),
		phillipSpectrumShader(new Shader(Utils::getRootFolder() + "PhillipsSpectrum.cso")),
		conjugatedCalcCS(new Shader(Utils::getRootFolder() + "ConjugatedCalcCS.cso")),
		displacementShader(new Shader(Utils::getRootFolder() + "Displacement.cso")),
		ifftShader(new Shader(Utils::getRootFolder() + "IFFT.cso")),
		permutationCS(new Shader(Utils::getRootFolder() + "PermutationCS.cso")),
		waveMergeCS(new Shader(Utils::getRootFolder() + "WaveMergeCS.cso")),
		oceanVShader(new Shader(Utils::getRootFolder() + "OceanVShader.cso")),
		oceanHShader(new Shader(Utils::getRootFolder() + "OceanHShader.cso")),
		oceanDShader(new Shader(Utils::getRootFolder() + "OceanDShader.cso")),
		oceanPShader(new Shader(Utils::getRootFolder() + "OceanPShader.cso")),
		tildeh0k(ResourceManager::createTextureRenderTarget(1024, 1024, DXGI_FORMAT_R32G32_FLOAT, 1, 1, false, true,
			DXGI_FORMAT_R32G32_FLOAT, DXGI_FORMAT_R32G32_FLOAT, DXGI_FORMAT_UNKNOWN)),
		tildeh0(ResourceManager::createTextureRenderTarget(1024, 1024, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 1, false, true,
			DXGI_FORMAT_R32G32B32A32_FLOAT, DXGI_FORMAT_R32G32B32A32_FLOAT, DXGI_FORMAT_UNKNOWN)),
		waveData(ResourceManager::createTextureRenderTarget(1024, 1024, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 1, false, true,
			DXGI_FORMAT_R32G32B32A32_FLOAT, DXGI_FORMAT_R32G32B32A32_FLOAT, DXGI_FORMAT_UNKNOWN)),
		Dy(ResourceManager::createTextureRenderTarget(1024, 1024, DXGI_FORMAT_R32G32_FLOAT, 1, 1, false, true,
			DXGI_FORMAT_R32G32_FLOAT, DXGI_FORMAT_R32G32_FLOAT, DXGI_FORMAT_UNKNOWN)),
		Dx(ResourceManager::createTextureRenderTarget(1024, 1024, DXGI_FORMAT_R32G32_FLOAT, 1, 1, false, true,
			DXGI_FORMAT_R32G32_FLOAT, DXGI_FORMAT_R32G32_FLOAT, DXGI_FORMAT_UNKNOWN)),
		Dz(ResourceManager::createTextureRenderTarget(1024, 1024, DXGI_FORMAT_R32G32_FLOAT, 1, 1, false, true,
			DXGI_FORMAT_R32G32_FLOAT, DXGI_FORMAT_R32G32_FLOAT, DXGI_FORMAT_UNKNOWN)),
		Dyx(ResourceManager::createTextureRenderTarget(1024, 1024, DXGI_FORMAT_R32G32_FLOAT, 1, 1, false, true,
			DXGI_FORMAT_R32G32_FLOAT, DXGI_FORMAT_R32G32_FLOAT, DXGI_FORMAT_UNKNOWN)),
		Dyz(ResourceManager::createTextureRenderTarget(1024, 1024, DXGI_FORMAT_R32G32_FLOAT, 1, 1, false, true,
			DXGI_FORMAT_R32G32_FLOAT, DXGI_FORMAT_R32G32_FLOAT, DXGI_FORMAT_UNKNOWN)),
		Dxx(ResourceManager::createTextureRenderTarget(1024, 1024, DXGI_FORMAT_R32G32_FLOAT, 1, 1, false, true,
			DXGI_FORMAT_R32G32_FLOAT, DXGI_FORMAT_R32G32_FLOAT, DXGI_FORMAT_UNKNOWN)),
		Dzz(ResourceManager::createTextureRenderTarget(1024, 1024, DXGI_FORMAT_R32G32_FLOAT, 1, 1, false, true,
			DXGI_FORMAT_R32G32_FLOAT, DXGI_FORMAT_R32G32_FLOAT, DXGI_FORMAT_UNKNOWN)),
		Dxz(ResourceManager::createTextureRenderTarget(1024, 1024, DXGI_FORMAT_R32G32_FLOAT, 1, 1, false, true,
			DXGI_FORMAT_R32G32_FLOAT, DXGI_FORMAT_R32G32_FLOAT, DXGI_FORMAT_UNKNOWN)),
		tempTexture(ResourceManager::createTextureRenderTarget(1024, 1024, DXGI_FORMAT_R32G32_FLOAT, 1, 1, false, true,
			DXGI_FORMAT_R32G32_FLOAT, DXGI_FORMAT_R32G32_FLOAT, DXGI_FORMAT_UNKNOWN)),
		Dxyz(ResourceManager::createTextureRenderTarget(1024, 1024, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 1, false, true,
			DXGI_FORMAT_R32G32B32A32_FLOAT, DXGI_FORMAT_R32G32B32A32_FLOAT, DXGI_FORMAT_UNKNOWN)),
		normalJacobian(ResourceManager::createTextureRenderTarget(1024, 1024, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 1, false, true,
			DXGI_FORMAT_R32G32B32A32_FLOAT, DXGI_FORMAT_R32G32B32A32_FLOAT, DXGI_FORMAT_UNKNOWN)),
		originTexture(ResourceManager::createTextureRenderTarget(Graphics::getWidth(), Graphics::getHeight(), DXGI_FORMAT_R16G16B16A16_FLOAT, 1, 1, false, true,
			DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_R16G16B16A16_FLOAT)),
		depthTexture(ResourceManager::createTextureDepthStencil(Graphics::getWidth(), Graphics::getHeight(), DXGI_FORMAT_R32_TYPELESS, 1, 1, false, true))
	{
		{
			D3D12_GRAPHICS_PIPELINE_STATE_DESC desc = {};
			desc.InputLayout = {};
			desc.pRootSignature = GlobalRootSignature::getGraphicsRootSignature()->get();
			desc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
			desc.RasterizerState = States::rasterCullBack;
			desc.DepthStencilState.DepthEnable = false;
			desc.DepthStencilState.StencilEnable = false;
			desc.SampleMask = UINT_MAX;
			desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
			desc.NumRenderTargets = 1;
			desc.RTVFormats[0] = originTexture->getTexture()->getFormat();
			desc.SampleDesc.Count = 1;
			desc.VS = Shader::textureCubeVS->getByteCode();
			desc.PS = textureCubePS->getByteCode();

			GraphicsDevice::get()->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(&skyboxState));
		}

		{
			D3D12_COMPUTE_PIPELINE_STATE_DESC desc = {};
			desc.pRootSignature = GlobalRootSignature::getComputeRootSignature()->get();
			desc.CS = phillipSpectrumShader->getByteCode();

			GraphicsDevice::get()->CreateComputePipelineState(&desc, IID_PPV_ARGS(&phillipSpectrumState));
		}

		{
			D3D12_COMPUTE_PIPELINE_STATE_DESC desc = {};
			desc.pRootSignature = GlobalRootSignature::getComputeRootSignature()->get();
			desc.CS = conjugatedCalcCS->getByteCode();

			GraphicsDevice::get()->CreateComputePipelineState(&desc, IID_PPV_ARGS(&conjugatedCalcState));
		}

		{
			D3D12_COMPUTE_PIPELINE_STATE_DESC desc = {};
			desc.pRootSignature = GlobalRootSignature::getComputeRootSignature()->get();
			desc.CS = displacementShader->getByteCode();

			GraphicsDevice::get()->CreateComputePipelineState(&desc, IID_PPV_ARGS(&displacementState));
		}

		{
			D3D12_COMPUTE_PIPELINE_STATE_DESC desc = {};
			desc.pRootSignature = GlobalRootSignature::getComputeRootSignature()->get();
			desc.CS = ifftShader->getByteCode();

			GraphicsDevice::get()->CreateComputePipelineState(&desc, IID_PPV_ARGS(&ifftState));
		}

		{
			D3D12_COMPUTE_PIPELINE_STATE_DESC desc = {};
			desc.pRootSignature = GlobalRootSignature::getComputeRootSignature()->get();
			desc.CS = permutationCS->getByteCode();

			GraphicsDevice::get()->CreateComputePipelineState(&desc, IID_PPV_ARGS(&permutationState));
		}

		{
			D3D12_COMPUTE_PIPELINE_STATE_DESC desc = {};
			desc.pRootSignature = GlobalRootSignature::getComputeRootSignature()->get();
			desc.CS = waveMergeCS->getByteCode();

			GraphicsDevice::get()->CreateComputePipelineState(&desc, IID_PPV_ARGS(&waveMergeState));
		}

		{
			D3D12_INPUT_ELEMENT_DESC inputLayoutDesc[2] =
			{
				{"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},
				{"TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0}
			};

			D3D12_GRAPHICS_PIPELINE_STATE_DESC desc = {};
			desc.InputLayout = { inputLayoutDesc,_countof(inputLayoutDesc) };
			desc.pRootSignature = GlobalRootSignature::getGraphicsRootSignature()->get();
			desc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
			desc.RasterizerState = States::rasterCullBack;
			desc.DepthStencilState = States::depthLess;
			desc.SampleMask = UINT_MAX;
			desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_PATCH;
			desc.NumRenderTargets = 1;
			desc.RTVFormats[0] = originTexture->getTexture()->getFormat();
			desc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
			desc.SampleDesc.Count = 1;
			desc.VS = oceanVShader->getByteCode();
			desc.HS = oceanHShader->getByteCode();
			desc.DS = oceanDShader->getByteCode();
			desc.PS = oceanPShader->getByteCode();

			GraphicsDevice::get()->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(&oceanState));
		}

		{
			D3D12_GRAPHICS_PIPELINE_STATE_DESC desc = {};
			desc.InputLayout = {};
			desc.pRootSignature = GlobalRootSignature::getGraphicsRootSignature()->get();
			desc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
			desc.RasterizerState = States::rasterCullBack;
			desc.DepthStencilState.DepthEnable = false;
			desc.DepthStencilState.StencilEnable = false;
			desc.SampleMask = UINT_MAX;
			desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
			desc.NumRenderTargets = 1;
			desc.RTVFormats[0] = Graphics::BackBufferFormat;
			desc.SampleDesc.Count = 1;
			desc.VS = Shader::fullScreenVS->getByteCode();
			desc.PS = Shader::fullScreenPS->getByteCode();

			GraphicsDevice::get()->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(&fullScreenState));
		}

		normalJacobian->getTexture()->getResource()->SetName(L"Normal Jacobian");

		Dxyz->getTexture()->getResource()->SetName(L"Dxyz");

		originTexture->getTexture()->getResource()->SetName(L"Origin Texture");

		std::vector<Vertex> vertices;

		{
			auto getVertexAt = [this](const int& x, const int& z)
				{
					const float xPos = ((float)x - (float)patchSize / 2.f) * (float)param.mapLength / (float)patchSize - (float)param.mapLength / 2.f;
					const float zPos = ((float)z - (float)patchSize / 2.f) * (float)param.mapLength / (float)patchSize - (float)param.mapLength / 2.f;
					const float u = (float)x / (float)patchSize;
					const float v = (float)z / (float)patchSize;
					return Vertex{ {xPos,0.f,zPos},{u,v} };
				};

			for (int z = 0; z < tildeNum * 32; z++)
			{
				for (int x = 0; x < tildeNum * 32; x++)
				{
					vertices.push_back(getVertexAt(x, z));
					vertices.push_back(getVertexAt(x, z + 1));
					vertices.push_back(getVertexAt(x + 1, z + 1));
					vertices.push_back(getVertexAt(x + 1, z));
				}
			}
		}

		begin();

		effect = new BloomEffect(context, Graphics::getWidth(), Graphics::getHeight(), resManager);

		envCube = resManager->createTextureCube("E:\\Assets\\Ocean\\ColdSunsetEquirect.png", 1024, true);

		randomGauss = resManager->createTextureRenderTarget(1024, 1024, RandomDataType::GAUSS, true);

		patchVertexBuffer = resManager->createVertexBuffer(sizeof(Vertex), sizeof(Vertex) * vertices.size(), false, vertices.data());

		calPhillipsTexture();

		end();

		RenderEngine::get()->submitRenderPass(this);
	}

	~MyRenderPass()
	{
		delete oceanParamBuffer;

		delete textureCubePS;
		delete phillipSpectrumShader;
		delete conjugatedCalcCS;
		delete displacementShader;
		delete ifftShader;
		delete permutationCS;
		delete waveMergeCS;
		delete oceanVShader;
		delete oceanHShader;
		delete oceanDShader;
		delete oceanPShader;

		delete patchVertexBuffer;

		delete envCube;
		delete randomGauss;

		delete tildeh0k;
		delete tildeh0;
		delete waveData;
		delete Dy;
		delete Dx;
		delete Dz;
		delete Dyx;
		delete Dyz;
		delete Dxx;
		delete Dzz;
		delete Dxz;
		delete tempTexture;
		delete Dxyz;
		delete normalJacobian;

		delete originTexture;
		delete depthTexture;

		delete effect;
	}

protected:

	void calPhillipsTexture() const
	{
		oceanParamBuffer->update(&param, sizeof(Param));

		context->setPipelineState(phillipSpectrumState.Get());
		context->setCSConstants({
			tildeh0k->getUAVMipIndex(0),
			waveData->getUAVMipIndex(0),
			randomGauss->getAllSRVIndex() }
		, 0);
		context->setCSConstantBuffer(oceanParamBuffer);
		context->transitionResources();
		context->dispatch(1024 / 32, 1024 / 32, 1);
		context->uavBarrier({
			tildeh0k->getTexture(),
			waveData->getTexture()
			});

		context->setPipelineState(conjugatedCalcState.Get());
		context->setCSConstants({
			tildeh0->getUAVMipIndex(0),
			tildeh0k->getAllSRVIndex()
			}
		, 0);
		context->transitionResources();
		context->dispatch(1024 / 32, 1024 / 32, 1);
		context->uavBarrier({
			tildeh0->getTexture()
			});
	}

	void ifft(TextureRenderTarget* const inputTexture)
	{
		context->setPipelineState(ifftState.Get());

		context->setCSConstants({
			tempTexture->getUAVMipIndex(0),
			inputTexture->getAllSRVIndex()
			}
		, 0);
		context->transitionResources();
		context->dispatch(1024, 1, 1);
		context->uavBarrier({
			tempTexture->getTexture()
			});

		context->setCSConstants({
			inputTexture->getUAVMipIndex(0),
			tempTexture->getAllSRVIndex()
			}
		, 0);
		context->transitionResources();
		context->dispatch(1024, 1, 1);
		context->uavBarrier({
			inputTexture->getTexture()
			});

		context->setPipelineState(permutationState.Get());
		context->setCSConstants({
			inputTexture->getUAVMipIndex(0)
			}
		, 0);
		context->transitionResources();
		context->dispatch(1024 / 32, 1024 / 32, 1);
		context->uavBarrier({
			inputTexture->getTexture()
			});
	}

	void recordCommand() override
	{
		context->setPipelineState(displacementState.Get());
		context->setCSConstants({
			Dy->getUAVMipIndex(0),
			Dx->getUAVMipIndex(0),
			Dz->getUAVMipIndex(0),
			Dyx->getUAVMipIndex(0),
			Dyz->getUAVMipIndex(0),
			Dxx->getUAVMipIndex(0),
			Dzz->getUAVMipIndex(0),
			Dxz->getUAVMipIndex(0),
			tildeh0->getAllSRVIndex(),
			waveData->getAllSRVIndex()
			}
		, 0);
		context->transitionResources();
		context->dispatch(1024 / 32, 1024 / 32, 1);
		context->uavBarrier({
			Dy->getTexture(),
			Dx->getTexture(),
			Dz->getTexture(),
			Dyx->getTexture(),
			Dyz->getTexture(),
			Dxx->getTexture(),
			Dzz->getTexture(),
			Dxz->getTexture()
			}
		);

		ifft(Dy);
		ifft(Dx);
		ifft(Dz);
		ifft(Dyx);
		ifft(Dyz);
		ifft(Dxx);
		ifft(Dzz);
		ifft(Dxz);

		context->setPipelineState(waveMergeState.Get());
		context->setCSConstants({
			Dxyz->getUAVMipIndex(0),
			normalJacobian->getUAVMipIndex(0),
			Dy->getAllSRVIndex(),
			Dx->getAllSRVIndex(),
			Dz->getAllSRVIndex(),
			Dyx->getAllSRVIndex(),
			Dyz->getAllSRVIndex(),
			Dxx->getAllSRVIndex(),
			Dzz->getAllSRVIndex(),
			Dxz->getAllSRVIndex()
			}
		, 0);
		context->transitionResources();
		context->dispatch(1024 / 32, 1024 / 32, 1);
		context->uavBarrier({
			Dxyz->getTexture(),
			normalJacobian->getTexture()
			});

		context->setPipelineState(skyboxState.Get());
		context->setViewport(Graphics::getWidth(), Graphics::getHeight());
		context->setScissorRect(0, 0, Graphics::getWidth(), Graphics::getHeight());
		context->setTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		context->setRenderTargets({ originTexture->getRTVMipHandle(0) }, {});

		context->setPSConstants({ envCube->getAllSRVIndex() }, 0);

		context->transitionResources();

		context->draw(36, 1, 0, 0);

		context->setPipelineState(oceanState.Get());
		context->setViewport(Graphics::getWidth(), Graphics::getHeight());
		context->setScissorRect(0, 0, Graphics::getWidth(), Graphics::getHeight());
		context->setTopology(D3D_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST);

		context->setVertexBuffers(0, { patchVertexBuffer->getVertexBuffer() });

		const DepthStencilDesc depthStencilDesc = depthTexture->getDSVMipHandle(0);

		context->setRenderTargets({ originTexture->getRTVMipHandle(0) }, &depthStencilDesc);

		context->setDSConstants({
			Dxyz->getAllSRVIndex()
			}
		, 0);
		context->setPSConstants({
			normalJacobian->getAllSRVIndex(),
			envCube->getAllSRVIndex()
			}
		, 0);
		context->transitionResources();

		context->clearDepthStencil(depthTexture->getDSVMipHandle(0), D3D12_CLEAR_FLAG_DEPTH, 1.f, 0);
		context->draw(4 * tildeNum * 32 * tildeNum * 32, 1, 0, 0);

		TextureRenderTarget* bloomTexture = effect->process(originTexture);

		context->setPipelineState(fullScreenState.Get());
		context->setViewport(Graphics::getWidth(), Graphics::getHeight());
		context->setScissorRect(0, 0, Graphics::getWidth(), Graphics::getHeight());
		context->setTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		context->setDefRenderTarget();
		context->setPSConstants({ bloomTexture->getAllSRVIndex() }, 0);
		context->transitionResources();
		context->draw(3, 1, 0, 0);
	}

private:

	static constexpr UINT tildeNum = 6;

	static constexpr UINT patchSize = 32;

	struct Vertex
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT2 uv;
	};

	const struct Param
	{
		unsigned int mapResolution = 1024;
		float mapLength = 512.0;
		DirectX::XMFLOAT2 wind = { 20.f,0.f };
		float amplitude = 0.000001f;
		float gravity = 9.81f;
		DirectX::XMFLOAT2 padding0;
		DirectX::XMFLOAT4 padding1[14];
	}param;

	Shader* textureCubePS;

	Shader* phillipSpectrumShader;

	Shader* conjugatedCalcCS;

	Shader* displacementShader;

	Shader* ifftShader;

	Shader* permutationCS;

	Shader* waveMergeCS;

	Shader* oceanVShader;

	Shader* oceanHShader;

	Shader* oceanDShader;

	Shader* oceanPShader;

	ComPtr<ID3D12PipelineState> skyboxState;

	ComPtr<ID3D12PipelineState> phillipSpectrumState;

	ComPtr<ID3D12PipelineState> conjugatedCalcState;

	ComPtr<ID3D12PipelineState> displacementState;

	ComPtr<ID3D12PipelineState> ifftState;

	ComPtr<ID3D12PipelineState> permutationState;

	ComPtr<ID3D12PipelineState> waveMergeState;

	ComPtr<ID3D12PipelineState> oceanState;

	ComPtr<ID3D12PipelineState> fullScreenState;

	TextureRenderTarget* envCube;

	TextureRenderTarget* randomGauss;

	VertexBuffer* patchVertexBuffer;

	ConstantBuffer* oceanParamBuffer;

	TextureRenderTarget* tildeh0k;

	TextureRenderTarget* tildeh0;

	TextureRenderTarget* waveData;

	TextureRenderTarget* tempTexture;

	TextureRenderTarget* Dy;

	TextureRenderTarget* Dx;

	TextureRenderTarget* Dz;

	TextureRenderTarget* Dyx;

	TextureRenderTarget* Dyz;

	TextureRenderTarget* Dxx;

	TextureRenderTarget* Dzz;

	TextureRenderTarget* Dxz;

	TextureRenderTarget* Dxyz;

	TextureRenderTarget* normalJacobian;

	TextureRenderTarget* originTexture;

	TextureDepthStencil* depthTexture;

	BloomEffect* effect;

};