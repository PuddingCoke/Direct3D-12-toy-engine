#pragma once

#include<Gear/Core/RenderPass.h>
#include<Gear/Core/Shader.h>

#include<Gear/Core/Effect/BloomEffect.h>

#include<DirectXColors.h>

#include"Scene.h"

class MyRenderPass :public RenderPass
{
public:

	MyRenderPass() :
		gPosition(ResourceManager::createTextureRenderTarget(Graphics::getWidth(), Graphics::getHeight(), DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 1, false, true,
			DXGI_FORMAT_R32G32B32A32_FLOAT, DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_R32G32B32A32_FLOAT, DirectX::Colors::Transparent)),
		gNormalSpecular(ResourceManager::createTextureRenderTarget(Graphics::getWidth(), Graphics::getHeight(), DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 1, false, true,
			DXGI_FORMAT_R32G32B32A32_FLOAT, DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_R32G32B32A32_FLOAT, DirectX::Colors::Transparent)),
		gBaseColor(ResourceManager::createTextureRenderTarget(Graphics::getWidth(), Graphics::getHeight(), DXGI_FORMAT_R8G8B8A8_UNORM, 1, 1, false, true,
			DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_R8G8B8A8_UNORM, DirectX::Colors::Transparent)),
		depthTexture(ResourceManager::createTextureDepthStencil(Graphics::getWidth(), Graphics::getHeight(), DXGI_FORMAT_R32_TYPELESS, 1, 1, false, true)),
		shadowTexture(ResourceManager::createTextureDepthStencil(shadowTextureResolution, shadowTextureResolution, DXGI_FORMAT_R32_TYPELESS, 1, 1, false, true)),
		originTexture(ResourceManager::createTextureRenderTarget(Graphics::getWidth(), Graphics::getHeight(), DXGI_FORMAT_R16G16B16A16_FLOAT, 1, 1, false, true,
			DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_R16G16B16A16_FLOAT, DirectX::Colors::Black)),
		radianceCube(ResourceManager::createTextureRenderTarget(probeCaptureResolution, probeCaptureResolution, DXGI_FORMAT_R16G16B16A16_FLOAT, 6, 1, true, true,
			DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_R16G16B16A16_FLOAT, radianceCubeClearColor)),
		distanceCube(ResourceManager::createTextureRenderTarget(probeCaptureResolution, probeCaptureResolution, DXGI_FORMAT_R32_FLOAT, 6, 1, true, true,
			DXGI_FORMAT_R32_FLOAT, DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_R32_FLOAT, distanceCubeClearColor)),
		depthCube(ResourceManager::createTextureDepthStencil(probeCaptureResolution, probeCaptureResolution, DXGI_FORMAT_R32_TYPELESS, 6, 1, true, true)),
		irradianceVolumeBuffer(ResourceManager::createConstantBuffer(sizeof(IrradianceVolume), true)),
		shadowVS(new Shader(Utils::getRootFolder() + "ShadowVS.cso")),
		deferredVShader(new Shader(Utils::getRootFolder() + "DeferredVShader.cso")),
		deferredPShader(new Shader(Utils::getRootFolder() + "DeferredPShader.cso")),
		deferredFinal(new Shader(Utils::getRootFolder() + "DeferredFinal.cso")),
		cubeRenderVS(new Shader(Utils::getRootFolder() + "CubeRenderVS.cso")),
		cubeRenderPS(new Shader(Utils::getRootFolder() + "CubeRenderPS.cso")),
		cubeRenderBouncePS(new Shader(Utils::getRootFolder() + "CubeRenderBouncePS.cso")),
		irradianceCompute(new Shader(Utils::getRootFolder() + "IrradianceCompute.cso")),
		octahedralEncode(new Shader(Utils::getRootFolder() + "OctahedralEncode.cso")),
		skyboxPShader(new Shader(Utils::getRootFolder() + "SkybosPShader.cso")),
		sunAngle(Math::half_pi - 0.01f)
	{
		const UINT probeCount = irradianceVolume.count.x * irradianceVolume.count.y * irradianceVolume.count.z;

		irradianceCoeff = ResourceManager::createTextureRenderTarget(9, 1, DXGI_FORMAT_R11G11B10_FLOAT, probeCount, 1, false, true,
			DXGI_FORMAT_R11G11B10_FLOAT, DXGI_FORMAT_R11G11B10_FLOAT, DXGI_FORMAT_UNKNOWN);

		irradianceBounceCoeff = ResourceManager::createTextureRenderTarget(9, 1, DXGI_FORMAT_R11G11B10_FLOAT, probeCount, 1, false, true,
			DXGI_FORMAT_R11G11B10_FLOAT, DXGI_FORMAT_R11G11B10_FLOAT, DXGI_FORMAT_UNKNOWN);

		depthOctahedralMap = ResourceManager::createTextureRenderTarget(16, 16, DXGI_FORMAT_R16G16_FLOAT, probeCount, 1, false, true,
			DXGI_FORMAT_R16G16_FLOAT, DXGI_FORMAT_R16G16_FLOAT, DXGI_FORMAT_UNKNOWN);

		irradianceCoeff->getTexture()->getResource()->SetName(L"Irradiance Coeff");

		irradianceBounceCoeff->getTexture()->getResource()->SetName(L"Irradiance Bounce Coeff");

		depthOctahedralMap->getTexture()->getResource()->SetName(L"Depth Octahedral Map");

		shadowTexture->getTexture()->getResource()->SetName(L"Shadow Texture");

		radianceCube->getTexture()->getResource()->SetName(L"Radiance Cube");

		distanceCube->getTexture()->getResource()->SetName(L"Distance Cube");

		{
			D3D12_GRAPHICS_PIPELINE_STATE_DESC desc = PipelineState::getDefaultGraphicsDesc();
			desc.InputLayout = { inputDesc,_countof(inputDesc) };
			desc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
			desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
			desc.RasterizerState = States::rasterShadow;
			desc.DepthStencilState = States::depthLess;
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
			desc.RasterizerState = States::rasterCullBack;
			desc.DepthStencilState = States::depthLess;
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
			D3D12_GRAPHICS_PIPELINE_STATE_DESC desc = PipelineState::getDefaultFullScreenState();
			desc.NumRenderTargets = 1;
			desc.RTVFormats[0] = Graphics::BackBufferFormat;
			desc.PS = Shader::fullScreenPS->getByteCode();

			GraphicsDevice::get()->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(&fullScreenBlitState));
		}

		{
			D3D12_GRAPHICS_PIPELINE_STATE_DESC desc = PipelineState::getDefaultGraphicsDesc();
			desc.InputLayout = { inputDesc,_countof(inputDesc) };
			desc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
			desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
			desc.RasterizerState = States::rasterCullBack;
			desc.DepthStencilState = States::depthLess;
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
			desc.RasterizerState = States::rasterCullBack;
			desc.DepthStencilState = States::depthLess;
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
			desc.RasterizerState = States::rasterCullBack;
			desc.DepthStencilState = States::depthLessEqual;
			desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
			desc.NumRenderTargets = 1;
			desc.RTVFormats[0] = DXGI_FORMAT_R16G16B16A16_FLOAT;
			desc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
			desc.VS = Shader::textureCubeVS->getByteCode();
			desc.PS = skyboxPShader->getByteCode();

			GraphicsDevice::get()->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(&skyboxState));
		}

		{
			D3D12_COMPUTE_PIPELINE_STATE_DESC desc = PipelineState::getDefaultComputeDesc();
			desc.CS = irradianceCompute->getByteCode();

			GraphicsDevice::get()->CreateComputePipelineState(&desc, IID_PPV_ARGS(&irradianceComputeState));
		}

		{
			D3D12_COMPUTE_PIPELINE_STATE_DESC desc = PipelineState::getDefaultComputeDesc();
			desc.CS = octahedralEncode->getByteCode();

			GraphicsDevice::get()->CreateComputePipelineState(&desc, IID_PPV_ARGS(&octahedralEncodeState));
		}

		begin();

		skybox = resManager->createTextureCube(assetPath + "/sky/kloppenheim_05_4k.hdr", 1024, true);

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

		{
			struct Sample
			{
				DirectX::XMFLOAT4 direction;
				float Ylm[9];
			};

			const unsigned int sampleCount = 64;
			const unsigned int sampleCountSqrt = 8;
			const double oneoverN = 1.0 / (double)sampleCountSqrt;

			unsigned int i = 0;

			std::vector<Sample> samples(sampleCount);

			for (unsigned int a = 0; a < sampleCountSqrt; a++)
			{
				for (unsigned int b = 0; b < sampleCountSqrt; b++)
				{
					const double x = (a + Random::Double()) * oneoverN;
					const double y = (b + Random::Double()) * oneoverN;
					const double theta = 2.0 * acos(sqrt(1.0 - x));
					const double phi = 2.0 * 3.141592653589793238 * y;

					samples[i].direction = DirectX::XMFLOAT4((float)sin(theta) * (float)cos(phi), (float)sin(theta) * (float)sin(phi), (float)cos(theta), 1.0);

					for (int l = 0; l < 3; l++)
					{
						for (int m = -l; m <= l; m++)
						{
							int index = l * (l + 1) + m;
							samples[i].Ylm[index] = (float)SH(l, m, theta, phi);
						}
					}
					++i;
				}
			}

			irradianceSamples = resManager->ResourceManager::createStructuredBuffer(sizeof(Sample), sizeof(Sample) * samples.size(), false, samples.data(), true);

			irradianceSamples->getBuffer()->getResource()->SetName(L"Irradiance Samples");
		}

		effect = new BloomEffect(context, Graphics::getWidth(), Graphics::getHeight(), resManager);

		effect->setIntensity(0.5f);

		scene = new Scene(assetPath + "/sponza.dae", resManager);

		updateShadow();

		irradianceVolumeBuffer->update(&irradianceVolume, sizeof(IrradianceVolume));

		updateLightProbe();

		updateLightBounceProbe();

		end();

		RenderEngine::get()->submitRenderPass(this);
	}

	~MyRenderPass()
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

		delete irradianceCoeff;
		delete irradianceBounceCoeff;
		delete depthOctahedralMap;

		delete skybox;

		delete irradianceSamples;

		delete shadowVS;
		delete deferredVShader;
		delete deferredPShader;
		delete deferredFinal;
		delete cubeRenderVS;
		delete cubeRenderPS;
		delete cubeRenderBouncePS;
		delete skyboxPShader;

		delete effect;
	}

protected:

	static constexpr UINT shadowTextureResolution = 4096;

	static constexpr UINT probeCaptureResolution = 64;

	static constexpr float radianceCubeClearColor[4] = { 0.f,0.f,0.f,1.f };

	static constexpr float distanceCubeClearColor[4] = { 512.f,512.f,512.f,512.f };

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
		for (UINT x = 0; x < irradianceVolume.count.x; x++)
		{
			for (UINT z = 0; z < irradianceVolume.count.z; z++)
			{
				for (UINT y = 0; y < irradianceVolume.count.y; y++)
				{
					RenderCubeAt({ x,y,z });
				}
			}
		}
	}

	void updateLightBounceProbe()
	{
		for (UINT x = 0; x < irradianceVolume.count.x; x++)
		{
			for (UINT z = 0; z < irradianceVolume.count.z; z++)
			{
				for (UINT y = 0; y < irradianceVolume.count.y; y++)
				{
					RenderCubeBounceAt({ x,y,z });
				}
			}
		}
	}

	void RenderCubeAt(const DirectX::XMUINT3& probeGridPos)
	{
		const UINT probeIndex = ProbeGridPosToIndex(probeGridPos);

		context->setViewport(probeCaptureResolution, probeCaptureResolution);

		context->setScissorRect(0, 0, probeCaptureResolution, probeCaptureResolution);

		const DepthStencilDesc dsDesc = depthCube->getDSVMipHandle(0);

		context->setRenderTargets({ radianceCube->getRTVMipHandle(0),distanceCube->getRTVMipHandle(0) }, &dsDesc);

		context->setVSConstantBuffer(cubeRenderParamBuffer[probeIndex]);

		context->setPSConstants({
			shadowTexture->getAllDepthIndex(),
			irradianceVolumeBuffer->getBufferIndex()
			}, 3);

		context->setPSConstantBuffer(cubeRenderParamBuffer[probeIndex]);

		context->transitionResources();

		context->clearRenderTarget(radianceCube->getRTVMipHandle(0), radianceCubeClearColor);

		context->clearRenderTarget(distanceCube->getRTVMipHandle(0), distanceCubeClearColor);

		context->clearDepthStencil(dsDesc, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0);

		scene->renderCube(context, probeCapturePipelineState.Get());

		context->setPipelineState(irradianceComputeState.Get());

		context->setCSConstants({
			irradianceCoeff->getUAVMipIndex(0),
			radianceCube->getAllSRVIndex(),
			irradianceSamples->getBufferIndex()
			}, 0);

		context->setCSConstantBuffer(cubeRenderParamBuffer[probeIndex]);

		context->transitionResources();

		context->dispatch(1, 1, 1);

		context->uavBarrier({ irradianceCoeff->getTexture() });

		context->setPipelineState(octahedralEncodeState.Get());

		context->setCSConstants({
			depthOctahedralMap->getUAVMipIndex(0),
			distanceCube->getAllSRVIndex()
			}, 0);

		context->setCSConstantBuffer(cubeRenderParamBuffer[probeIndex]);

		context->transitionResources();

		context->dispatch(1, 1, 1);

		context->uavBarrier({ depthOctahedralMap->getTexture() });
	}

	void RenderCubeBounceAt(const DirectX::XMUINT3& probeGridPos)
	{
		const UINT probeIndex = ProbeGridPosToIndex(probeGridPos);

		context->setViewport(probeCaptureResolution, probeCaptureResolution);

		context->setScissorRect(0, 0, probeCaptureResolution, probeCaptureResolution);

		const DepthStencilDesc dsDesc = depthCube->getDSVMipHandle(0);

		context->setRenderTargets({ radianceCube->getRTVMipHandle(0) }, &dsDesc);

		context->setVSConstantBuffer(cubeRenderParamBuffer[probeIndex]);

		context->setPSConstants({
			shadowTexture->getAllDepthIndex(),
			irradianceVolumeBuffer->getBufferIndex(),
			irradianceCoeff->getAllSRVIndex(),
			depthOctahedralMap->getAllSRVIndex()
			}, 3);

		context->setPSConstantBuffer(cubeRenderParamBuffer[probeIndex]);

		context->transitionResources();

		context->clearRenderTarget(radianceCube->getRTVMipHandle(0), radianceCubeClearColor);

		context->clearDepthStencil(dsDesc, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0);

		scene->renderCube(context, probeCaptureBouncePipelineState.Get());

		context->setPipelineState(irradianceComputeState.Get());

		context->setCSConstants({
			irradianceBounceCoeff->getUAVMipIndex(0),
			radianceCube->getAllSRVIndex(),
			irradianceSamples->getBufferIndex()
			}, 0);

		context->setCSConstantBuffer(cubeRenderParamBuffer[probeIndex]);

		context->transitionResources();

		context->dispatch(1, 1, 1);

		context->uavBarrier({ irradianceBounceCoeff->getTexture() });
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
			irradianceBounceCoeff->getAllSRVIndex(),
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

		TextureRenderTarget* bloomTexture = effect->process(originTexture);

		context->setPipelineState(fullScreenBlitState.Get());

		context->setViewport(Graphics::getWidth(), Graphics::getHeight());

		context->setScissorRect(0, 0, Graphics::getWidth(), Graphics::getHeight());

		context->setTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		context->setDefRenderTarget();

		context->setPSConstants({ bloomTexture->getAllSRVIndex() }, 0);

		context->transitionResources();

		context->clearDefRenderTarget(DirectX::Colors::Black);

		context->draw(3, 1, 0, 0);
	}

	double Fact(int n)
	{
		const double results[13] = { 1.0,1.0,2.0,6.0,24.0,120.0,720.0,5040.0,40320.0,362880.0,3628800.0,39916800.0,479001600.0 };
		return results[n];
	}

	double K(int l, int m)
	{
		double temp = ((2.0 * l + 1.0) * Fact(l - m)) / (4.0 * 3.141592653589793238 * Fact(l + m));
		return sqrt(temp);
	}

	double P(int l, int m, double x)
	{
		double pmm = 1.0;
		if (m > 0)
		{
			double somx2 = sqrt((1.0 - x) * (1.0 + x));
			double fact = 1.0;
			for (int i = 1; i <= m; i++) {
				pmm *= (-fact) * somx2;
				fact += 2.0;
			}
		}
		if (l == m)
			return pmm;

		double pmmp1 = x * (2.0 * m + 1.0) * pmm;
		if (l == m + 1)
			return pmmp1;

		double pll = 0.0;
		for (int ll = m + 2; ll <= l; ++ll)
		{
			pll = ((2.0 * ll - 1.0) * x * pmmp1 - (ll + m - 1.0) * pmm) / (ll - m);
			pmm = pmmp1;
			pmmp1 = pll;
		}

		return pll;
	}

	double SH(int l, int m, double theta, double phi)
	{
		const double sqrt2 = sqrt(2.0);
		if (m == 0)        return K(l, 0) * P(l, 0, cos(theta));
		else if (m > 0)    return sqrt2 * K(l, m) * cos(m * phi) * P(l, m, cos(theta));
		else                return sqrt2 * K(l, -m) * sin(-m * phi) * P(l, -m, cos(theta));
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

	struct IrradianceVolume
	{
		DirectX::XMVECTOR lightDir;
		DirectX::XMVECTOR lightColor;
		DirectX::XMMATRIX lightViewProj;
		DirectX::XMFLOAT3 start = { -142.f,-16.f,-74.f };
		float spacing = 18.2f;
		DirectX::XMUINT3 count = { 17,9,12 };
		float irradianceDistanceBias = 0.f;
		float irradianceVarianceBias = 0.f;
		float irradianceChebyshevBias = 0.0f;
		DirectX::XMFLOAT2 padding0;
		DirectX::XMFLOAT4 padding1[7];
	} irradianceVolume;

	ConstantBuffer* irradianceVolumeBuffer;

	ConstantBuffer* cubeRenderParamBuffer[17 * 9 * 12];

	float sunAngle;

	TextureRenderTarget* gPosition;

	TextureRenderTarget* gNormalSpecular;

	TextureRenderTarget* gBaseColor;

	TextureDepthStencil* depthTexture;

	TextureDepthStencil* shadowTexture;

	TextureRenderTarget* originTexture;

	TextureRenderTarget* radianceCube;

	TextureRenderTarget* distanceCube;

	TextureDepthStencil* depthCube;

	TextureRenderTarget* irradianceCoeff;

	TextureRenderTarget* irradianceBounceCoeff;

	TextureRenderTarget* depthOctahedralMap;

	StructuredBuffer* irradianceSamples;

	TextureRenderTarget* skybox;

	ComPtr<ID3D12PipelineState> shadowPipelineState;

	ComPtr<ID3D12PipelineState> deferredPipelineState;

	ComPtr<ID3D12PipelineState> deferredFinalPipelineState;

	ComPtr<ID3D12PipelineState> fullScreenBlitState;

	ComPtr<ID3D12PipelineState> probeCapturePipelineState;

	ComPtr<ID3D12PipelineState> probeCaptureBouncePipelineState;

	ComPtr<ID3D12PipelineState> irradianceComputeState;

	ComPtr<ID3D12PipelineState> octahedralEncodeState;

	ComPtr<ID3D12PipelineState> skyboxState;

	Shader* shadowVS;

	Shader* deferredVShader;

	Shader* deferredPShader;

	Shader* deferredFinal;

	Shader* cubeRenderVS;

	Shader* cubeRenderPS;

	Shader* cubeRenderBouncePS;

	Shader* irradianceCompute;

	Shader* octahedralEncode;

	Shader* skyboxPShader;

	BloomEffect* effect;

};