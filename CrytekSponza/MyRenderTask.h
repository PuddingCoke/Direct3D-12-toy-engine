#pragma once

#include<Gear/Core/RenderTask.h>
#include<Gear/Core/Shader.h>

#include<Gear/Core/Effect/BloomEffect.h>
#include<Gear/Core/Effect/FXAAEffect.h>

#include<DirectXColors.h>

#include"Scene.h"

class MyRenderTask :public RenderTask
{
public:

	MyRenderTask() :
		gPosition(ResourceManager::createTextureRenderView(Graphics::getWidth(), Graphics::getHeight(), DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 1, false, true,
			DXGI_FORMAT_R32G32B32A32_FLOAT, DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_R32G32B32A32_FLOAT, DirectX::Colors::Transparent)),
		gNormalSpecular(ResourceManager::createTextureRenderView(Graphics::getWidth(), Graphics::getHeight(), DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 1, false, true,
			DXGI_FORMAT_R32G32B32A32_FLOAT, DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_R32G32B32A32_FLOAT, DirectX::Colors::Transparent)),
		gBaseColor(ResourceManager::createTextureRenderView(Graphics::getWidth(), Graphics::getHeight(), DXGI_FORMAT_R8G8B8A8_UNORM, 1, 1, false, true,
			DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_R8G8B8A8_UNORM, DirectX::Colors::Transparent)),
		depthTexture(ResourceManager::createTextureDepthView(Graphics::getWidth(), Graphics::getHeight(), DXGI_FORMAT_R32_TYPELESS, 1, 1, false, true)),
		shadowTexture(ResourceManager::createTextureDepthView(shadowTextureResolution, shadowTextureResolution, DXGI_FORMAT_R32_TYPELESS, 1, 1, false, true)),
		originTexture(ResourceManager::createTextureRenderView(Graphics::getWidth(), Graphics::getHeight(), DXGI_FORMAT_R16G16B16A16_FLOAT, 1, 1, false, true,
			DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_R16G16B16A16_FLOAT, DirectX::Colors::Black)),
		radianceCube(ResourceManager::createTextureRenderView(probeCaptureResolution, probeCaptureResolution, DXGI_FORMAT_R16G16B16A16_FLOAT, 6, 1, true, true,
			DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_R16G16B16A16_FLOAT, radianceCubeClearColor)),
		distanceCube(ResourceManager::createTextureRenderView(probeCaptureResolution, probeCaptureResolution, DXGI_FORMAT_R32_FLOAT, 6, 1, true, true,
			DXGI_FORMAT_R32_FLOAT, DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_R32_FLOAT, distanceCubeClearColor)),
		depthCube(ResourceManager::createTextureDepthView(probeCaptureResolution, probeCaptureResolution, DXGI_FORMAT_D32_FLOAT, 6, 1, true, true)),
		irradianceVolumeBuffer(ResourceManager::createConstantBuffer(sizeof(IrradianceVolume), true)),
		shadowVS(new Shader(Utils::getRootFolder() + L"ShadowVS.cso")),
		deferredVShader(new Shader(Utils::getRootFolder() + L"DeferredVShader.cso")),
		deferredPShader(new Shader(Utils::getRootFolder() + L"DeferredPShader.cso")),
		deferredFinal(new Shader(Utils::getRootFolder() + L"DeferredFinal.cso")),
		cubeRenderVS(new Shader(Utils::getRootFolder() + L"CubeRenderVS.cso")),
		cubeRenderPS(new Shader(Utils::getRootFolder() + L"CubeRenderPS.cso")),
		cubeRenderBouncePS(new Shader(Utils::getRootFolder() + L"CubeRenderBouncePS.cso")),
		irradianceOctahedralEncode(new Shader(Utils::getRootFolder() + L"IrradianceOctahedralEncode.cso")),
		depthOctahedralEncode(new Shader(Utils::getRootFolder() + L"DepthOctahedralEncode.cso")),
		skyboxPShader(new Shader(Utils::getRootFolder() + L"SkybosPShader.cso")),
		sunAngle(Math::half_pi - 0.01f)
	{
		irradianceOctahedralMap = ResourceManager::createTextureRenderView(6, 6, DXGI_FORMAT_R11G11B10_FLOAT, probeCount, 1, false, true,
			DXGI_FORMAT_R11G11B10_FLOAT, DXGI_FORMAT_R11G11B10_FLOAT, DXGI_FORMAT_UNKNOWN);

		irradianceBounceOctahedralMap = ResourceManager::createTextureRenderView(6, 6, DXGI_FORMAT_R11G11B10_FLOAT, probeCount, 1, false, true,
			DXGI_FORMAT_R11G11B10_FLOAT, DXGI_FORMAT_R11G11B10_FLOAT, DXGI_FORMAT_UNKNOWN);

		depthOctahedralMap = ResourceManager::createTextureRenderView(16, 16, DXGI_FORMAT_R16G16_FLOAT, probeCount, 1, false, true,
			DXGI_FORMAT_R16G16_FLOAT, DXGI_FORMAT_R16G16_FLOAT, DXGI_FORMAT_UNKNOWN);

		irradianceOctahedralMap->getTexture()->setName(L"Irradiance Octahedral Map");

		irradianceBounceOctahedralMap->getTexture()->setName(L"Irradiance Bounce Octahedral Map");

		depthOctahedralMap->getTexture()->setName(L"Depth Octahedral Map");

		shadowTexture->getTexture()->setName(L"Shadow Texture");

		radianceCube->getTexture()->setName(L"Radiance Cube");

		distanceCube->getTexture()->setName(L"Distance Cube");

		{
			D3D12_GRAPHICS_PIPELINE_STATE_DESC desc = PipelineState::getDefaultGraphicsDesc();
			desc.InputLayout = { inputDesc,_countof(inputDesc) };
			desc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
			desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
			desc.RasterizerState = PipelineState::rasterShadow;
			desc.DepthStencilState = PipelineState::depthLess;
			desc.NumRenderTargets = 0;
			desc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
			desc.VS = shadowVS->getByteCode();

			GraphicsDevice::get()->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(&shadowPipelineState));
		}

		{
			D3D12_GRAPHICS_PIPELINE_STATE_DESC desc = PipelineState::getDefaultGraphicsDesc();
			desc.InputLayout = { inputDesc,_countof(inputDesc) };
			desc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
			desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
			desc.RasterizerState = PipelineState::rasterCullBack;
			desc.DepthStencilState = PipelineState::depthLess;
			desc.NumRenderTargets = 3;
			desc.RTVFormats[0] = DXGI_FORMAT_R32G32B32A32_FLOAT;
			desc.RTVFormats[1] = DXGI_FORMAT_R32G32B32A32_FLOAT;
			desc.RTVFormats[2] = DXGI_FORMAT_R8G8B8A8_UNORM;
			desc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
			desc.VS = deferredVShader->getByteCode();
			desc.PS = deferredPShader->getByteCode();

			GraphicsDevice::get()->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(&deferredPipelineState));
		}

		{
			D3D12_GRAPHICS_PIPELINE_STATE_DESC desc = PipelineState::getDefaultFullScreenState();
			desc.NumRenderTargets = 1;
			desc.RTVFormats[0] = DXGI_FORMAT_R16G16B16A16_FLOAT;
			desc.PS = deferredFinal->getByteCode();

			GraphicsDevice::get()->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(&deferredFinalPipelineState));
		}

		{
			D3D12_GRAPHICS_PIPELINE_STATE_DESC desc = PipelineState::getDefaultGraphicsDesc();
			desc.InputLayout = { inputDesc,_countof(inputDesc) };
			desc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
			desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
			desc.RasterizerState = PipelineState::rasterCullBack;
			desc.DepthStencilState = PipelineState::depthLess;
			desc.NumRenderTargets = 2;
			desc.RTVFormats[0] = DXGI_FORMAT_R16G16B16A16_FLOAT;
			desc.RTVFormats[1] = DXGI_FORMAT_R32_FLOAT;
			desc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
			desc.VS = cubeRenderVS->getByteCode();
			desc.PS = cubeRenderPS->getByteCode();

			GraphicsDevice::get()->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(&probeCapturePipelineState));
		}

		{
			D3D12_GRAPHICS_PIPELINE_STATE_DESC desc = PipelineState::getDefaultGraphicsDesc();
			desc.InputLayout = { inputDesc,_countof(inputDesc) };
			desc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
			desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
			desc.RasterizerState = PipelineState::rasterCullBack;
			desc.DepthStencilState = PipelineState::depthLess;
			desc.NumRenderTargets = 1;
			desc.RTVFormats[0] = DXGI_FORMAT_R16G16B16A16_FLOAT;
			desc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
			desc.VS = cubeRenderVS->getByteCode();
			desc.PS = cubeRenderBouncePS->getByteCode();

			GraphicsDevice::get()->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(&probeCaptureBouncePipelineState));
		}

		{
			D3D12_GRAPHICS_PIPELINE_STATE_DESC desc = PipelineState::getDefaultGraphicsDesc();
			desc.InputLayout = {};
			desc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
			desc.RasterizerState = PipelineState::rasterCullBack;
			desc.DepthStencilState = PipelineState::depthLessEqual;
			desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
			desc.NumRenderTargets = 1;
			desc.RTVFormats[0] = DXGI_FORMAT_R16G16B16A16_FLOAT;
			desc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
			desc.VS = Shader::textureCubeVS->getByteCode();
			desc.PS = skyboxPShader->getByteCode();

			GraphicsDevice::get()->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(&skyboxState));
		}

		PipelineState::createComputeState(&irradianceOctahedralEncodeState, irradianceOctahedralEncode);

		PipelineState::createComputeState(&depthOctahedralEncodeState, depthOctahedralEncode);

		skybox = resManager->createTextureCube(wAssetPath + L"/sky/kloppenheim_05_4k.hdr", 1024, true);

		{
			struct CubeRenderParam
			{
				DirectX::XMMATRIX viewProj[6];
				DirectX::XMFLOAT3 probeLocation;
				UINT probeIndex;
				DirectX::XMFLOAT4 padding[7];
			} cubeRenderParam{};

			for (UINT x = 0; x < irradianceVolume.count.x; x++)
			{
				for (UINT z = 0; z < irradianceVolume.count.z; z++)
				{
					for (UINT y = 0; y < irradianceVolume.count.y; y++)
					{
						const DirectX::XMFLOAT3 location = ProbeGridPosToLoc({ x,y,z });

						const UINT probeIndex = ProbeGridPosToIndex({ x,y,z });

						const DirectX::XMVECTOR focusPoints[6] =
						{
							{1.0f,  0.0f,  0.0f},
							{-1.0f,  0.0f,  0.0f},
							{0.0f,  1.0f,  0.0f},
							{0.0f, -1.0f,  0.0f},
							{0.0f,  0.0f,  1.0f},
							{0.0f,  0.0f, -1.0f}
						};
						const DirectX::XMVECTOR upVectors[6] =
						{
							{0.0f, 1.0f,  0.0f},
							{0.0f, 1.0f,  0.0f},
							{0.0f,  0.0f,  -1.0f},
							{0.0f,  0.0f, 1.0f},
							{0.0f, 1.0f,  0.0f},
							{0.0f, 1.0f,  0.0f}
						};

						const DirectX::XMMATRIX projMatrix = DirectX::XMMatrixPerspectiveFovLH(Math::pi / 2.f, 1.f, 1.f, 512.f);

						for (int i = 0; i < 6; i++)
						{
							const DirectX::XMMATRIX viewMatrix = DirectX::XMMatrixLookAtLH(DirectX::XMLoadFloat3(&location), DirectX::XMVectorAdd(focusPoints[i], DirectX::XMLoadFloat3(&location)), upVectors[i]);
							cubeRenderParam.viewProj[i] = DirectX::XMMatrixTranspose(viewMatrix * projMatrix);
						}

						cubeRenderParam.probeLocation = location;
						cubeRenderParam.probeIndex = probeIndex;

						cubeRenderParamBuffer[probeIndex] = resManager->createConstantBuffer(sizeof(CubeRenderParam), false, &cubeRenderParam, true);
					}
				}
			}
		}

		bloomEffect = new BloomEffect(context, Graphics::getWidth(), Graphics::getHeight(), resManager);

		bloomEffect->setIntensity(0.5f);

		bloomEffect->setExposure(0.574f);

		bloomEffect->setGamma(1.578f);

		fxaaEffect = new FXAAEffect(context, Graphics::getWidth(), Graphics::getHeight());

		scene = new Scene(assetPath + "/sponza.dae", resManager);

		updateLightField();
	}

	~MyRenderTask()
	{
		delete scene;

		delete irradianceVolumeBuffer;

		for (UINT i = 0; i < 17 * 9 * 12; i++)
		{
			delete cubeRenderParamBuffer[i];
		}

		delete gPosition;
		delete gNormalSpecular;
		delete gBaseColor;
		delete depthTexture;
		delete shadowTexture;
		delete originTexture;

		delete radianceCube;
		delete distanceCube;
		delete depthCube;

		delete irradianceOctahedralMap;
		delete irradianceBounceOctahedralMap;
		delete depthOctahedralMap;

		delete skybox;

		delete shadowVS;
		delete deferredVShader;
		delete deferredPShader;
		delete deferredFinal;
		delete cubeRenderVS;
		delete cubeRenderPS;
		delete cubeRenderBouncePS;
		delete skyboxPShader;

		delete bloomEffect;
		delete fxaaEffect;
	}

	void imGUICall() override
	{
		bloomEffect->imGUICall();

		fxaaEffect->imGUICall();
	}

protected:

	static constexpr UINT shadowTextureResolution = 4096;

	static constexpr UINT probeCaptureResolution = 64;

	static constexpr float radianceCubeClearColor[4] = { 0.f,0.f,0.f,1.f };

	static constexpr float distanceCubeClearColor[4] = { 512.f,512.f,512.f,512.f };

	void updateLightField()
	{
		updateShadow();

		irradianceVolumeBuffer->update(&irradianceVolume, sizeof(IrradianceVolume));

		updateLightProbe();
	}

	void updateShadow()
	{
		const float xSize = 183;
		const float ySize = 130;
		const float distance = 260.f;
		const DirectX::XMVECTOR offset = { 6.5f,0.f,0.f };

		irradianceVolume.lightDir = { 0.f,sinf(sunAngle),cosf(sunAngle),0.f };

		irradianceVolume.lightColor = DirectX::Colors::White;

		irradianceVolume.lightDir = DirectX::XMVector3Normalize(irradianceVolume.lightDir);

		const DirectX::XMVECTOR lightCamPos = DirectX::XMVectorAdd(DirectX::XMVectorScale(irradianceVolume.lightDir, distance), offset);
		const DirectX::XMMATRIX lightProjMat = DirectX::XMMatrixOrthographicLH(xSize, ySize, 1.f, 512.f);
		const DirectX::XMMATRIX lightViewMat = DirectX::XMMatrixLookAtLH(lightCamPos, offset, { 0.f,1.f,0.f });
		const DirectX::XMMATRIX lightMat = DirectX::XMMatrixTranspose(lightViewMat * lightProjMat);

		irradianceVolume.lightViewProj = lightMat;

		const DepthStencilDesc dsDesc = shadowTexture->getDSVMipHandle(0);

		context->setRenderTargets({}, &dsDesc);

		context->setViewport(shadowTextureResolution, shadowTextureResolution);

		context->setScissorRect(0, 0, shadowTextureResolution, shadowTextureResolution);

		context->setVSConstantBuffer(irradianceVolumeBuffer);

		context->transitionResources();

		context->clearDepthStencil(dsDesc, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0);

		scene->render(context, shadowPipelineState.Get());
	}

	void updateLightProbe()
	{
		for (UINT i = 0; i < probeCount; i++)
		{
			renderCubeAt(cubeRenderParamBuffer[i]);
		}

		for (UINT i = 0; i < probeCount; i++)
		{
			renderCubeBounceAt(cubeRenderParamBuffer[i]);
		}
	}

	void renderCubeAt(const ConstantBuffer* const cubeRenderBuffer)
	{
		context->setViewport(probeCaptureResolution, probeCaptureResolution);

		context->setScissorRect(0, 0, probeCaptureResolution, probeCaptureResolution);

		const DepthStencilDesc dsDesc = depthCube->getDSVMipHandle(0);

		context->setRenderTargets({ radianceCube->getRTVMipHandle(0),distanceCube->getRTVMipHandle(0) }, &dsDesc);

		context->setVSConstantBuffer(cubeRenderBuffer);

		context->setPSConstants({
			shadowTexture->getAllDepthIndex(),
			irradianceVolumeBuffer->getBufferIndex()
			}, 3);

		context->setPSConstantBuffer(cubeRenderBuffer);

		context->transitionResources();

		context->clearRenderTarget(radianceCube->getRTVMipHandle(0), radianceCubeClearColor);

		context->clearRenderTarget(distanceCube->getRTVMipHandle(0), distanceCubeClearColor);

		context->clearDepthStencil(dsDesc, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0);

		scene->renderCube(context, probeCapturePipelineState.Get());

		context->setPipelineState(irradianceOctahedralEncodeState.Get());

		context->setCSConstants({
			irradianceOctahedralMap->getUAVMipIndex(0),
			radianceCube->getAllSRVIndex()
			}, 0);

		context->setCSConstantBuffer(cubeRenderBuffer);

		context->transitionResources();

		context->dispatch(1, 1, 1);

		context->uavBarrier({ irradianceOctahedralMap->getTexture() });

		context->setPipelineState(depthOctahedralEncodeState.Get());

		context->setCSConstants({
			depthOctahedralMap->getUAVMipIndex(0),
			distanceCube->getAllSRVIndex()
			}, 0);

		context->setCSConstantBuffer(cubeRenderBuffer);

		context->transitionResources();

		context->dispatch(1, 1, 1);

		context->uavBarrier({ depthOctahedralMap->getTexture() });
	}

	void renderCubeBounceAt(const ConstantBuffer* const cubeRenderBuffer)
	{
		context->setViewport(probeCaptureResolution, probeCaptureResolution);

		context->setScissorRect(0, 0, probeCaptureResolution, probeCaptureResolution);

		const DepthStencilDesc dsDesc = depthCube->getDSVMipHandle(0);

		context->setRenderTargets({ radianceCube->getRTVMipHandle(0) }, &dsDesc);

		context->setVSConstantBuffer(cubeRenderBuffer);

		context->setPSConstants({
			shadowTexture->getAllDepthIndex(),
			irradianceVolumeBuffer->getBufferIndex(),
			irradianceOctahedralMap->getAllSRVIndex(),
			depthOctahedralMap->getAllSRVIndex()
			}, 3);

		context->setPSConstantBuffer(cubeRenderBuffer);

		context->transitionResources();

		context->clearRenderTarget(radianceCube->getRTVMipHandle(0), radianceCubeClearColor);

		context->clearDepthStencil(dsDesc, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0);

		scene->renderCube(context, probeCaptureBouncePipelineState.Get());

		context->setPipelineState(irradianceOctahedralEncodeState.Get());

		context->setCSConstants({
			irradianceBounceOctahedralMap->getUAVMipIndex(0),
			radianceCube->getAllSRVIndex()
			}, 0);

		context->setCSConstantBuffer(cubeRenderBuffer);

		context->transitionResources();

		context->dispatch(1, 1, 1);

		context->uavBarrier({ irradianceBounceOctahedralMap->getTexture() });
	}

	void recordCommand() override
	{
		context->setViewport(Graphics::getWidth(), Graphics::getHeight());

		context->setScissorRect(0, 0, Graphics::getWidth(), Graphics::getHeight());

		const DepthStencilDesc dsDesc = depthTexture->getDSVMipHandle(0);

		context->setRenderTargets({
			gPosition->getRTVMipHandle(0),
			gNormalSpecular->getRTVMipHandle(0),
			gBaseColor->getRTVMipHandle(0)
			}, &dsDesc);

		context->transitionResources();

		context->clearRenderTarget(gPosition->getRTVMipHandle(0), DirectX::Colors::Transparent);

		context->clearRenderTarget(gNormalSpecular->getRTVMipHandle(0), DirectX::Colors::Transparent);

		context->clearRenderTarget(gBaseColor->getRTVMipHandle(0), DirectX::Colors::Transparent);

		context->clearDepthStencil(dsDesc, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0);

		scene->render(context, deferredPipelineState.Get());

		context->setPipelineState(deferredFinalPipelineState.Get());

		context->setViewport(Graphics::getWidth(), Graphics::getHeight());

		context->setScissorRect(0, 0, Graphics::getWidth(), Graphics::getHeight());

		context->setTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		context->setRenderTargets({ originTexture->getRTVMipHandle(0) });

		context->setPSConstants({
			gPosition->getAllSRVIndex(),
			gNormalSpecular->getAllSRVIndex(),
			gBaseColor->getAllSRVIndex(),
			shadowTexture->getAllDepthIndex(),
			irradianceBounceOctahedralMap->getAllSRVIndex(),
			depthOctahedralMap->getAllSRVIndex()
			}, 0);

		context->setPSConstantBuffer(irradianceVolumeBuffer);

		context->transitionResources();

		context->clearRenderTarget(originTexture->getRTVMipHandle(0), DirectX::Colors::Black);

		context->draw(3, 1, 0, 0);

		context->setPipelineState(skyboxState.Get());

		context->setViewport(Graphics::getWidth(), Graphics::getHeight());

		context->setScissorRect(0, 0, Graphics::getWidth(), Graphics::getHeight());

		context->setTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		context->setRenderTargets({ originTexture->getRTVMipHandle(0) }, &dsDesc);

		context->setPSConstants({ skybox->getAllSRVIndex() }, 0);

		context->transitionResources();

		context->draw(36, 1, 0, 0);

		TextureRenderView* const bloomTexture = bloomEffect->process(originTexture);

		TextureRenderView* const fxaaTexture = fxaaEffect->process(bloomTexture);

		blit(fxaaTexture);
	}

	UINT ProbeGridPosToIndex(const DirectX::XMUINT3& probeGridPos)
	{
		return probeGridPos.x + probeGridPos.z * irradianceVolume.count.x + probeGridPos.y * irradianceVolume.count.x * irradianceVolume.count.z;
	}

	DirectX::XMFLOAT3 ProbeGridPosToLoc(const DirectX::XMUINT3& probeGridPos)
	{
		return {
			probeGridPos.x * irradianceVolume.spacing + irradianceVolume.start.x,
			probeGridPos.y * irradianceVolume.spacing + irradianceVolume.start.y,
			probeGridPos.z * irradianceVolume.spacing + irradianceVolume.start.z,
		};
	}

	Scene* scene;

	static constexpr UINT probeCountX = 17;

	static constexpr UINT probeCountY = 9;

	static constexpr UINT probeCountZ = 12;

	static constexpr UINT probeCount = probeCountX * probeCountY * probeCountZ;

	struct IrradianceVolume
	{
		DirectX::XMVECTOR lightDir;
		DirectX::XMVECTOR lightColor;
		DirectX::XMMATRIX lightViewProj;
		DirectX::XMFLOAT3 start = { -142.f,-16.f,-74.f };
		float spacing = 18.2f;
		const DirectX::XMUINT3 count = { probeCountX,probeCountY,probeCountZ };
		float irradianceDistanceBias = 0.f;
		float irradianceVarianceBias = 0.f;
		float irradianceChebyshevBias = 0.0f;
		DirectX::XMFLOAT2 padding0;
		DirectX::XMFLOAT4 padding1[7];
	} irradianceVolume;

	ConstantBuffer* irradianceVolumeBuffer;

	ConstantBuffer* cubeRenderParamBuffer[probeCount];

	float sunAngle;

	TextureRenderView* gPosition;

	TextureRenderView* gNormalSpecular;

	TextureRenderView* gBaseColor;

	TextureDepthView* depthTexture;

	TextureDepthView* shadowTexture;

	TextureRenderView* originTexture;

	TextureRenderView* radianceCube;

	TextureRenderView* distanceCube;

	TextureDepthView* depthCube;

	TextureRenderView* irradianceOctahedralMap;

	TextureRenderView* irradianceBounceOctahedralMap;

	TextureRenderView* depthOctahedralMap;

	TextureRenderView* skybox;

	ComPtr<ID3D12PipelineState> shadowPipelineState;

	ComPtr<ID3D12PipelineState> deferredPipelineState;

	ComPtr<ID3D12PipelineState> deferredFinalPipelineState;

	ComPtr<ID3D12PipelineState> probeCapturePipelineState;

	ComPtr<ID3D12PipelineState> probeCaptureBouncePipelineState;

	ComPtr<ID3D12PipelineState> irradianceOctahedralEncodeState;

	ComPtr<ID3D12PipelineState> depthOctahedralEncodeState;

	ComPtr<ID3D12PipelineState> skyboxState;

	Shader* shadowVS;

	Shader* deferredVShader;

	Shader* deferredPShader;

	Shader* deferredFinal;

	Shader* cubeRenderVS;

	Shader* cubeRenderPS;

	Shader* cubeRenderBouncePS;

	Shader* irradianceOctahedralEncode;

	Shader* depthOctahedralEncode;

	Shader* skyboxPShader;

	BloomEffect* bloomEffect;

	FXAAEffect* fxaaEffect;

};