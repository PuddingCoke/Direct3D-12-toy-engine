#pragma once

#include<Gear/Core/RenderPass.h>

#include<Gear/Core/Effect/BloomEffect.h>

#include<Gear/Utils/Color.h>

#include<DirectXColors.h>

class MyRenderPass :public RenderPass
{
public:

	MyRenderPass() :
		colorUpdateTimer(1.f),
		splatVelocityCS(new Shader(Utils::getRootFolder() + "SplatVelocityCS.cso")),
		splatColorCS(new Shader(Utils::getRootFolder() + "SplatColorCS.cso")),
		divergenceCS(new Shader(Utils::getRootFolder() + "DivergenceCS.cso")),
		pressureResetCS(new Shader(Utils::getRootFolder() + "PressureResetCS.cso")),
		pressureCS(new Shader(Utils::getRootFolder() + "PressureCS.cso")),
		gradientSubtractCS(new Shader(Utils::getRootFolder() + "GradientSubtractCS.cso")),
		velocityAdvectionCS(new Shader(Utils::getRootFolder() + "VelocityAdvectionCS.cso")),
		colorAdvectionCS(new Shader(Utils::getRootFolder() + "ColorAdvectionCS.cso")),
		velocityBoundaryCS(new Shader(Utils::getRootFolder() + "VelocityBoundaryCS.cso")),
		pressureBoundaryCS(new Shader(Utils::getRootFolder() + "PressureBoundaryCS.cso")),
		fluidFinalCS(new Shader(Utils::getRootFolder() + "FluidFinalCS.cso"))
	{
		const DirectX::XMUINT2 simRes = { Graphics::getWidth() / config.resolutionFactor,Graphics::getHeight() / config.resolutionFactor };

		velocityTex = new SwapTexture([=] {return ResourceManager::createTextureRenderView(simRes.x, simRes.y, DXGI_FORMAT_R32G32_FLOAT, 1, 1, false, true,
			DXGI_FORMAT_R32G32_FLOAT, DXGI_FORMAT_R32G32_FLOAT, DXGI_FORMAT_UNKNOWN); });

		velocityTex->read()->getTexture()->getResource()->SetName(L"Velocity Texture (0)");

		velocityTex->write()->getTexture()->getResource()->SetName(L"Velocity Texture (1)");

		colorTex = new SwapTexture([=] {return ResourceManager::createTextureRenderView(Graphics::getWidth(), Graphics::getHeight(), DXGI_FORMAT_R16G16B16A16_FLOAT, 1, 1, false, true,
			DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_UNKNOWN); });

		colorTex->read()->getTexture()->getResource()->SetName(L"Color Texture (0)");

		colorTex->write()->getTexture()->getResource()->SetName(L"Color Texture (1)");

		divergenceTex = ResourceManager::createTextureRenderView(simRes.x, simRes.y, DXGI_FORMAT_R32_FLOAT, 1, 1, false, true,
			DXGI_FORMAT_R32_FLOAT, DXGI_FORMAT_R32_FLOAT, DXGI_FORMAT_UNKNOWN);

		divergenceTex->getTexture()->getResource()->SetName(L"Divergence Texture");

		pressureTex = new SwapTexture([=] {return ResourceManager::createTextureRenderView(simRes.x, simRes.y, DXGI_FORMAT_R32_FLOAT, 1, 1, false, true,
			DXGI_FORMAT_R32_FLOAT, DXGI_FORMAT_R32_FLOAT, DXGI_FORMAT_UNKNOWN); });

		pressureTex->read()->getTexture()->getResource()->SetName(L"Pressure Texture (0)");

		pressureTex->write()->getTexture()->getResource()->SetName(L"Pressure Texture (1)");

		curlTex = ResourceManager::createTextureRenderView(simRes.x, simRes.y, DXGI_FORMAT_R32_FLOAT, 1, 1, false, true,
			DXGI_FORMAT_R32_FLOAT, DXGI_FORMAT_R32_FLOAT, DXGI_FORMAT_UNKNOWN);

		originTexture = ResourceManager::createTextureRenderView(Graphics::getWidth(), Graphics::getHeight(), DXGI_FORMAT_R16G16B16A16_FLOAT, 1, 1, false, true,
			DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_UNKNOWN);

		simulationParamBuffer = ResourceManager::createConstantBuffer(sizeof(SimulationParam), true);

		simulationParam.colorTexelSize = DirectX::XMFLOAT2(1.f / Graphics::getWidth(), 1.f / Graphics::getHeight());

		simulationParam.simTexelSize = DirectX::XMFLOAT2(1.f / simRes.x, 1.f / simRes.y);

		simulationParam.colorTextureSize = DirectX::XMUINT2(Graphics::getWidth(), Graphics::getHeight());

		simulationParam.simTextureSize = DirectX::XMUINT2(simRes.x, simRes.y);

		simulationParam.colorDissipationSpeed = config.colorDissipationSpeed;

		simulationParam.velocityDissipationSpeed = config.velocityDissipationSpeed;

		simulationParam.curlIntensity = config.curlIntensity;

		simulationParam.splatRadius = config.splatRadius / 100.f * Graphics::getAspectRatio();

		{
			D3D12_COMPUTE_PIPELINE_STATE_DESC desc = PipelineState::getDefaultComputeDesc();
			desc.CS = splatVelocityCS->getByteCode();

			GraphicsDevice::get()->CreateComputePipelineState(&desc, IID_PPV_ARGS(&splatVelocityState));
		}

		{
			D3D12_COMPUTE_PIPELINE_STATE_DESC desc = PipelineState::getDefaultComputeDesc();
			desc.CS = splatColorCS->getByteCode();

			GraphicsDevice::get()->CreateComputePipelineState(&desc, IID_PPV_ARGS(&splatColorState));
		}

		{
			D3D12_COMPUTE_PIPELINE_STATE_DESC desc = PipelineState::getDefaultComputeDesc();
			desc.CS = divergenceCS->getByteCode();

			GraphicsDevice::get()->CreateComputePipelineState(&desc, IID_PPV_ARGS(&divergenceState));
		}

		{
			D3D12_COMPUTE_PIPELINE_STATE_DESC desc = PipelineState::getDefaultComputeDesc();
			desc.CS = pressureResetCS->getByteCode();

			GraphicsDevice::get()->CreateComputePipelineState(&desc, IID_PPV_ARGS(&pressureResetState));
		}

		{
			D3D12_COMPUTE_PIPELINE_STATE_DESC desc = PipelineState::getDefaultComputeDesc();
			desc.CS = pressureCS->getByteCode();

			GraphicsDevice::get()->CreateComputePipelineState(&desc, IID_PPV_ARGS(&pressureState));
		}

		{
			D3D12_COMPUTE_PIPELINE_STATE_DESC desc = PipelineState::getDefaultComputeDesc();
			desc.CS = gradientSubtractCS->getByteCode();

			GraphicsDevice::get()->CreateComputePipelineState(&desc, IID_PPV_ARGS(&gradientSubtractState));
		}

		{
			D3D12_COMPUTE_PIPELINE_STATE_DESC desc = PipelineState::getDefaultComputeDesc();
			desc.CS = velocityAdvectionCS->getByteCode();

			GraphicsDevice::get()->CreateComputePipelineState(&desc, IID_PPV_ARGS(&velocityAdvectionState));
		}

		{
			D3D12_COMPUTE_PIPELINE_STATE_DESC desc = PipelineState::getDefaultComputeDesc();
			desc.CS = colorAdvectionCS->getByteCode();

			GraphicsDevice::get()->CreateComputePipelineState(&desc, IID_PPV_ARGS(&colorAdvectionState));
		}

		{
			D3D12_COMPUTE_PIPELINE_STATE_DESC desc = PipelineState::getDefaultComputeDesc();
			desc.CS = velocityBoundaryCS->getByteCode();

			GraphicsDevice::get()->CreateComputePipelineState(&desc, IID_PPV_ARGS(&velocityBoundaryState));
		}

		{
			D3D12_COMPUTE_PIPELINE_STATE_DESC desc = PipelineState::getDefaultComputeDesc();
			desc.CS = pressureBoundaryCS->getByteCode();

			GraphicsDevice::get()->CreateComputePipelineState(&desc, IID_PPV_ARGS(&pressureBoundaryState));
		}

		{
			D3D12_COMPUTE_PIPELINE_STATE_DESC desc = PipelineState::getDefaultComputeDesc();
			desc.CS = fluidFinalCS->getByteCode();

			GraphicsDevice::get()->CreateComputePipelineState(&desc, IID_PPV_ARGS(&fluidFinalState));
		}

		begin();

		effect = new BloomEffect(context, Graphics::getWidth(), Graphics::getHeight(), resManager);

		end();

		RenderEngine::get()->submitRenderPass(this);

		effect->setThreshold(0.f);
	}

	~MyRenderPass()
	{
		delete effect;

		delete colorTex;
		delete velocityTex;
		delete curlTex;
		delete divergenceTex;
		delete pressureTex;
		delete originTexture;

		delete simulationParamBuffer;

		delete splatVelocityCS;
		delete splatColorCS;
		delete divergenceCS;
		delete pressureResetCS;
		delete pressureCS;
		delete gradientSubtractCS;
		delete velocityAdvectionCS;
		delete colorAdvectionCS;
		delete velocityBoundaryCS;
		delete pressureBoundaryCS;
		delete fluidFinalCS;
	}

	void recordCommand() override
	{
		/*const DirectX::XMFLOAT2 pos =
		{
			(float)Mouse::getX() / Graphics::getWidth(),
			(float)(Graphics::getHeight() - Mouse::getY()) / Graphics::getHeight()
		};*/

		/*const DirectX::XMFLOAT2 posDelta =
		{
			(pos.x - simulationParam.pos.x) * config.splatForce,
			((pos.y - simulationParam.pos.y) / Graphics::getAspectRatio()) * config.splatForce
		};*/

		simulationParam.pos = DirectX::XMFLOAT2(0.10, 0.5);
		simulationParam.posDelta = DirectX::XMFLOAT2(30.0, 0.0);

		if (colorUpdateTimer.update(Graphics::getDeltaTime() * config.colorChangeSpeed))
		{
			Color c = Color::HSVtoRGB({ Random::Float(),1.f,1.f });
			simulationParam.splatColor = { c.r,c.g,c.b,1.f };
		}

		simulationParamBuffer->update(&simulationParam, sizeof(SimulationParam));

		context->setGlobalConstantBuffer(simulationParamBuffer);
		context->setTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		/*if (Mouse::onMove() && Mouse::getLeftDown())
		{*/

		context->setPipelineState(splatVelocityState.Get());
		context->setCSConstants({ velocityTex->read()->getAllSRVIndex(),velocityTex->write()->getUAVMipIndex(0) }, 0);
		context->transitionResources();
		context->dispatch(velocityTex->width / 16, velocityTex->height / 9, 1);
		context->uavBarrier({ velocityTex->write()->getTexture() });
		velocityTex->swap();

		//obstacle
		context->setPipelineState(velocityBoundaryState.Get());
		context->setCSConstants({ velocityTex->read()->getAllSRVIndex(),velocityTex->write()->getUAVMipIndex(0) }, 0);
		context->transitionResources();
		context->dispatch(velocityTex->width / 16, velocityTex->height / 9, 1);
		context->uavBarrier({ velocityTex->write()->getTexture() });
		velocityTex->swap();

		context->setPipelineState(splatColorState.Get());
		context->setCSConstants({ colorTex->read()->getAllSRVIndex(),colorTex->write()->getUAVMipIndex(0) }, 0);
		context->transitionResources();
		context->dispatch(colorTex->width / 16, colorTex->height / 9, 1);
		context->uavBarrier({ colorTex->write()->getTexture() });
		colorTex->swap();
		/*}*/

		context->setPipelineState(divergenceState.Get());
		context->setCSConstants({ velocityTex->read()->getAllSRVIndex(),divergenceTex->getUAVMipIndex(0) }, 0);
		context->transitionResources();
		context->dispatch(divergenceTex->getTexture()->getWidth() / 16, divergenceTex->getTexture()->getHeight() / 9, 1);
		context->uavBarrier({ divergenceTex->getTexture() });

		context->setPipelineState(pressureResetState.Get());
		context->setCSConstants({ pressureTex->write()->getUAVMipIndex(0) }, 0);
		context->transitionResources();
		context->dispatch(pressureTex->width / 16, pressureTex->height / 9, 1);
		context->uavBarrier({ pressureTex->write()->getTexture() });
		pressureTex->swap();

		for (UINT i = 0; i < config.pressureIteraion; i++)
		{
			context->setPipelineState(pressureState.Get());
			context->setCSConstants({ divergenceTex->getAllSRVIndex(),pressureTex->read()->getAllSRVIndex(),pressureTex->write()->getUAVMipIndex(0) }, 0);
			context->transitionResources();
			context->dispatch(pressureTex->width / 16, pressureTex->height / 9, 1);
			context->uavBarrier({ pressureTex->write()->getTexture() });
			pressureTex->swap();

			//obstacle
			context->setPipelineState(pressureBoundaryState.Get());
			context->setCSConstants({ pressureTex->read()->getAllSRVIndex(),pressureTex->write()->getUAVMipIndex(0) }, 0);
			context->transitionResources();
			context->dispatch(pressureTex->width / 16, pressureTex->height / 9, 1);
			context->uavBarrier({ pressureTex->write()->getTexture() });
			pressureTex->swap();
		}

		context->setPipelineState(gradientSubtractState.Get());
		context->setCSConstants({ pressureTex->read()->getAllSRVIndex(),velocityTex->read()->getAllSRVIndex(),velocityTex->write()->getUAVMipIndex(0) }, 0);
		context->transitionResources();
		context->dispatch(velocityTex->width / 16, velocityTex->height / 9, 1);
		context->uavBarrier({ velocityTex->write()->getTexture() });
		velocityTex->swap();

		//obstacle
		context->setPipelineState(velocityBoundaryState.Get());
		context->setCSConstants({ velocityTex->read()->getAllSRVIndex(),velocityTex->write()->getUAVMipIndex(0) }, 0);
		context->transitionResources();
		context->dispatch(velocityTex->width / 16, velocityTex->height / 9, 1);
		context->uavBarrier({ velocityTex->write()->getTexture() });
		velocityTex->swap();

		context->setPipelineState(velocityAdvectionState.Get());
		context->setCSConstants({ velocityTex->read()->getAllSRVIndex(),velocityTex->write()->getUAVMipIndex(0) }, 0);
		context->transitionResources();
		context->dispatch(velocityTex->width / 16, velocityTex->height / 9, 1);
		context->uavBarrier({ velocityTex->write()->getTexture() });
		velocityTex->swap();

		//obstacle
		context->setPipelineState(velocityBoundaryState.Get());
		context->setCSConstants({ velocityTex->read()->getAllSRVIndex(),velocityTex->write()->getUAVMipIndex(0) }, 0);
		context->transitionResources();
		context->dispatch(velocityTex->width / 16, velocityTex->height / 9, 1);
		context->uavBarrier({ velocityTex->write()->getTexture() });
		velocityTex->swap();

		context->setPipelineState(colorAdvectionState.Get());
		context->setCSConstants({ velocityTex->read()->getAllSRVIndex(),colorTex->read()->getAllSRVIndex(),colorTex->write()->getUAVMipIndex(0) }, 0);
		context->transitionResources();
		context->dispatch(colorTex->width / 16, colorTex->height / 9, 1);
		context->uavBarrier({ colorTex->write()->getTexture() });
		colorTex->swap();

		context->setPipelineState(fluidFinalState.Get());
		context->setCSConstants({ colorTex->read()->getAllSRVIndex(),originTexture->getUAVMipIndex(0) }, 0);
		context->transitionResources();
		context->dispatch(originTexture->getTexture()->getWidth() / 16, originTexture->getTexture()->getHeight() / 9, 1);
		context->uavBarrier({ originTexture->getTexture() });

		TextureRenderView* const outputTexture = effect->process(originTexture);

		blit(outputTexture);
	}

private:

	BloomEffect* effect;

	SwapTexture* colorTex;

	SwapTexture* velocityTex;

	TextureRenderView* curlTex;

	TextureRenderView* divergenceTex;

	SwapTexture* pressureTex;

	TextureRenderView* originTexture;

	struct Configuration
	{
		float colorChangeSpeed = 10.f;//颜色改变速度
		float colorDissipationSpeed = 0.1f;//颜色消散速度
		float velocityDissipationSpeed = 0.00f;//速度消散速度
		float curlIntensity = 80.f;//涡流强度
		float splatRadius = 0.40f;//施加颜色的半径
		float splatForce = 6000.f;//施加速度的强度
		const unsigned int pressureIteraion = 50;//雅可比迭代次数 这个值越高物理模拟越不容易出错 NVIDIA的文章有提到通常20-50次就够了
		const unsigned int resolutionFactor = 2;//物理模拟分辨率
	}config;

	struct SimulationParam
	{
		DirectX::XMFLOAT2 pos = { 0,0 };
		DirectX::XMFLOAT2 posDelta = { 0,0 };
		DirectX::XMFLOAT4 splatColor = { 0,0,0,1 };
		DirectX::XMFLOAT2 colorTexelSize;
		DirectX::XMFLOAT2 simTexelSize;
		DirectX::XMUINT2 colorTextureSize;
		DirectX::XMUINT2 simTextureSize;
		float colorDissipationSpeed;
		float velocityDissipationSpeed;
		float curlIntensity;
		float splatRadius;
		DirectX::XMFLOAT4 padding[11];
	} simulationParam;

	ConstantBuffer* simulationParamBuffer;

	Timer colorUpdateTimer;

	Shader* splatVelocityCS;

	ComPtr<ID3D12PipelineState> splatVelocityState;

	Shader* splatColorCS;

	ComPtr<ID3D12PipelineState> splatColorState;

	Shader* divergenceCS;

	ComPtr<ID3D12PipelineState> divergenceState;

	Shader* pressureResetCS;

	ComPtr<ID3D12PipelineState> pressureResetState;

	Shader* pressureCS;

	ComPtr<ID3D12PipelineState> pressureState;

	Shader* gradientSubtractCS;

	ComPtr<ID3D12PipelineState> gradientSubtractState;

	Shader* velocityAdvectionCS;

	ComPtr<ID3D12PipelineState> velocityAdvectionState;

	Shader* colorAdvectionCS;

	ComPtr<ID3D12PipelineState> colorAdvectionState;

	Shader* velocityBoundaryCS;

	ComPtr<ID3D12PipelineState> velocityBoundaryState;

	Shader* pressureBoundaryCS;

	ComPtr<ID3D12PipelineState> pressureBoundaryState;

	Shader* fluidFinalCS;

	ComPtr<ID3D12PipelineState> fluidFinalState;
};