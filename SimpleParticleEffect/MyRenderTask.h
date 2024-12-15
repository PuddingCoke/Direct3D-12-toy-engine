#pragma once

#include<Gear/Core/RenderTask.h>
#include<Gear/Core/Shader.h>

#include<Gear/Core/Effect/BloomEffect.h>
#include<Gear/Core/Effect/FXAAEffect.h>

#include<Gear/Utils/Color.h>

#include<Gear/Core/Resource/TextureRenderView.h>

class MyRenderTask :public RenderTask
{
public:

	MyRenderTask()
	{
		particleVS = new Shader("ParticleVS.hlsl", ShaderProfile::VERTEX);

		particleGS = new Shader("ParticleGS.hlsl", ShaderProfile::GEOMETRY);

		particlePS = new Shader("ParticlePS.hlsl", ShaderProfile::PIXEL);

		particleCS = new Shader("ParticleCS.hlsl", ShaderProfile::COMPUTE);

		PipelineState::createComputeState(&particleComputeState, particleCS);

		{
			D3D12_INPUT_ELEMENT_DESC inputDesc[2] =
			{
				{"POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
				{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}
			};

			D3D12_GRAPHICS_PIPELINE_STATE_DESC desc = PipelineState::getDefaultGraphicsDesc();
			desc.InputLayout = { inputDesc,_countof(inputDesc) };
			desc.VS = particleVS->getByteCode();
			desc.GS = particleGS->getByteCode();
			desc.PS = particlePS->getByteCode();
			desc.BlendState = States::addtiveBlendDesc;
			desc.RasterizerState = States::rasterCullBack;
			desc.DepthStencilState = States::depthLess;
			desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
			desc.NumRenderTargets = 1;
			desc.RTVFormats[0] = DXGI_FORMAT_R16G16B16A16_FLOAT;
			desc.DSVFormat = DXGI_FORMAT_D32_FLOAT;

			GraphicsDevice::get()->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(&particleRenderState));
		}

		bloomEffect = new BloomEffect(context, Graphics::getWidth(), Graphics::getHeight(), resManager);

		fxaaEffect = new FXAAEffect(context, Graphics::getWidth(), Graphics::getHeight());

		originTexture = ResourceManager::createTextureRenderView(Graphics::getWidth(), Graphics::getHeight(), DXGI_FORMAT_R16G16B16A16_FLOAT, 1, 1, false, true,
			DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_R16G16B16A16_FLOAT, DirectX::Colors::Black);

		depthTexture = ResourceManager::createTextureDepthView(Graphics::getWidth(), Graphics::getHeight(), DXGI_FORMAT_D32_FLOAT, 1, 1, false, true);

		{
			DirectX::XMFLOAT4* const positions = new DirectX::XMFLOAT4[numParticles];

			DirectX::XMFLOAT4* const colors = new DirectX::XMFLOAT4[numParticles];

			for (UINT i = 0; i < numParticles; i++)
			{
				const float radius = 0.3f * Random::Float() + 0.3f;

				const float theta = Random::Float() * Math::two_pi;

				const float phi = Random::Float() * Math::two_pi;

				positions[i] = DirectX::XMFLOAT4(radius * cosf(theta) * sinf(phi), radius * cosf(theta) * cosf(phi), radius * sinf(theta), 1.f);

				if (radius < 0.4f)
				{
					colors[i] = DirectX::XMFLOAT4(1.f, 1.f, 1.f, 1.f);
				}
				else
				{
					colors[i] = DirectX::XMFLOAT4(1.f, 1.f, radius * 0.4f + 0.1f, 1.f);
				}
			}

			positionBuffer = resManager->createTypedBufferView(DXGI_FORMAT_R32G32B32A32_FLOAT, sizeof(DirectX::XMFLOAT4) * numParticles, false, true, true, false, false, true, positions);

			colorBuffer = resManager->createTypedBufferView(DXGI_FORMAT_R32G32B32A32_FLOAT, sizeof(DirectX::XMFLOAT4) * numParticles, false, false, true, false, false, true, colors);
		}

		bloomEffect->setExposure(0.6f);

		bloomEffect->setThreshold(1.0f);

		simulationParam = { 0.18f,4 };
	}

	~MyRenderTask()
	{
		delete bloomEffect;
		delete fxaaEffect;

		delete particleVS;
		delete particleGS;
		delete particlePS;

		delete particleCS;

		delete originTexture;

		delete depthTexture;

		delete positionBuffer;
		delete colorBuffer;
	}

	void imGUICall() override
	{
		ImGui::Begin("Simulation Param & Particle Param");

		ImGui::SliderFloat("Dissipative Factor", &simulationParam.dissipativeFactor, 0.f, 0.2f);

		ImGui::SliderInt("Simulation Steps", &simulationParam.simulationSteps, 1, 8);

		ImGui::End();

		bloomEffect->imGUICall();

		fxaaEffect->imGUICall();
	}

protected:

	void recordCommand() override
	{
		context->setPipelineState(particleComputeState.Get());

		context->setCSConstants({ positionBuffer->getUAVIndex() }, 0);

		context->setCSConstants(sizeof(SimulationParam) / 4, &simulationParam, 1);

		context->transitionResources();

		context->dispatch(numParticles / 100, 1, 1);

		context->uavBarrier({ positionBuffer->getBuffer() });

		const DepthStencilDesc dsDesc = depthTexture->getDSVMipHandle(0);

		context->setPipelineState(particleRenderState.Get());

		context->setRenderTargets({ originTexture->getRTVMipHandle(0) }, &dsDesc);

		context->setViewportSimple(Graphics::getWidth(), Graphics::getHeight());

		context->setTopology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);

		context->setVertexBuffers(0, {
			positionBuffer->getVertexBuffer(),
			colorBuffer->getVertexBuffer()
			});

		context->setGSConstants(sizeof(SimulationParam) / 4, &simulationParam, 0);

		context->transitionResources();

		context->clearRenderTarget(originTexture->getRTVMipHandle(0), DirectX::Colors::Black);

		context->clearDepthStencil(dsDesc, D3D12_CLEAR_FLAG_DEPTH, 1.f, 0);

		context->draw(numParticles, 1, 0, 0);

		TextureRenderView* bloomTexture = bloomEffect->process(originTexture);

		TextureRenderView* fxaaTexture = fxaaEffect->process(bloomTexture);

		blit(fxaaTexture);
	}

private:

	constexpr static UINT numParticles = 50000;

	struct SimulationParam
	{
		float dissipativeFactor;
		int simulationSteps;
	} simulationParam;

	BloomEffect* bloomEffect;

	FXAAEffect* fxaaEffect;

	ComPtr<ID3D12PipelineState> particleComputeState;

	ComPtr<ID3D12PipelineState> particleRenderState;

	Shader* particleVS;

	Shader* particleGS;

	Shader* particlePS;

	Shader* particleCS;

	TextureRenderView* originTexture;

	TextureDepthView* depthTexture;

	BufferView* positionBuffer;

	BufferView* colorBuffer;

};