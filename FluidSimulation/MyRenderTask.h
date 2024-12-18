#pragma once

#include<Gear/Core/RenderTask.h>

#include<Gear/Core/Effect/BloomEffect.h>

#include<Gear/Utils/Color.h>

#include<DirectXColors.h>

class MyRenderTask :public RenderTask
{
public:

	MyRenderTask() :
		colorUpdateTimer(1.f),
		splatVelocityCS(new Shader(Utils::getRootFolder() + "SplatVelocityCS.cso")),
		splatColorCS(new Shader(Utils::getRootFolder() + "SplatColorCS.cso")),
		vorticityCS(new Shader(Utils::getRootFolder() + "VorticityCS.cso")),
		vorticityConfinementCS(new Shader(Utils::getRootFolder() + "VorticityConfinementCS.cso")),
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

		vorticityTex = ResourceManager::createTextureRenderView(simRes.x, simRes.y, DXGI_FORMAT_R32_FLOAT, 1, 1, false, true,
			DXGI_FORMAT_R32_FLOAT, DXGI_FORMAT_R32_FLOAT, DXGI_FORMAT_UNKNOWN);

		vorticityTex->getTexture()->getResource()->SetName(L"Vorticity Texture");

		originTexture = ResourceManager::createTextureRenderView(Graphics::getWidth(), Graphics::getHeight(), DXGI_FORMAT_R16G16B16A16_FLOAT, 1, 1, false, true,
			DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_UNKNOWN);

		simulationParamBuffer = ResourceManager::createConstantBuffer(sizeof(SimulationParam), true);

		simulationParam.colorTexelSize = DirectX::XMFLOAT2(1.f / Graphics::getWidth(), 1.f / Graphics::getHeight());

		simulationParam.simTexelSize = DirectX::XMFLOAT2(1.f / simRes.x, 1.f / simRes.y);

		simulationParam.colorTextureSize = DirectX::XMUINT2(Graphics::getWidth(), Graphics::getHeight());

		simulationParam.simTextureSize = DirectX::XMUINT2(simRes.x, simRes.y);

		simulationParam.colorDissipationSpeed = config.colorDissipationSpeed;

		simulationParam.velocityDissipationSpeed = config.velocityDissipationSpeed;

		simulationParam.vorticityIntensity = config.vorticityIntensity;

		simulationParam.splatRadius = config.splatRadius / 100.f * Graphics::getAspectRatio();

		PipelineState::createComputeState(&splatVelocityState, splatVelocityCS);

		PipelineState::createComputeState(&splatColorState, splatColorCS);

		PipelineState::createComputeState(&vorticityState, vorticityCS);

		PipelineState::createComputeState(&vorticityConfinementState, vorticityConfinementCS);

		PipelineState::createComputeState(&divergenceState, divergenceCS);

		PipelineState::createComputeState(&pressureResetState, pressureResetCS);

		PipelineState::createComputeState(&pressureState, pressureCS);

		PipelineState::createComputeState(&gradientSubtractState, gradientSubtractCS);

		PipelineState::createComputeState(&velocityAdvectionState, velocityAdvectionCS);

		PipelineState::createComputeState(&colorAdvectionState, colorAdvectionCS);

		PipelineState::createComputeState(&velocityBoundaryState, velocityBoundaryCS);

		PipelineState::createComputeState(&pressureBoundaryState, pressureBoundaryCS);

		PipelineState::createComputeState(&fluidFinalState, fluidFinalCS);

		effect = new BloomEffect(context, Graphics::getWidth(), Graphics::getHeight(), resManager);

		effect->setThreshold(0.f);
	}

	~MyRenderTask()
	{
		delete effect;

		delete colorTex;
		delete velocityTex;
		delete vorticityTex;
		delete divergenceTex;
		delete pressureTex;
		delete originTexture;

		delete simulationParamBuffer;

		delete splatVelocityCS;
		delete splatColorCS;
		delete vorticityCS;
		delete vorticityConfinementCS;
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

	void splatVelocityAndColor()
	{
		if (Mouse::onMove() && Mouse::getLeftDown())
		{
			context->setPipelineState(splatVelocityState.Get());
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
		}
	}

	void vorticityConfinement()
	{
		//calculate vorticity
		context->setPipelineState(vorticityState.Get());
		context->setCSConstants({ velocityTex->read()->getAllSRVIndex(),vorticityTex->getUAVMipIndex(0) }, 0);
		context->transitionResources();
		context->dispatch(vorticityTex->getTexture()->getWidth() / 16, vorticityTex->getTexture()->getHeight() / 9, 1);
		context->uavBarrier({ vorticityTex->getTexture() });

		//apply vorticity confinement
		context->setPipelineState(vorticityConfinementState.Get());
		context->setCSConstants({ vorticityTex->getAllSRVIndex(),velocityTex->read()->getAllSRVIndex(),velocityTex->write()->getUAVMipIndex(0) }, 0);
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
	}

	void project()
	{
		//calculate divergence
		context->setPipelineState(divergenceState.Get());
		context->setCSConstants({ velocityTex->read()->getAllSRVIndex(),divergenceTex->getUAVMipIndex(0) }, 0);
		context->transitionResources();
		context->dispatch(divergenceTex->getTexture()->getWidth() / 16, divergenceTex->getTexture()->getHeight() / 9, 1);
		context->uavBarrier({ divergenceTex->getTexture() });

		//reset pressure
		context->setPipelineState(pressureResetState.Get());
		context->setCSConstants({ pressureTex->write()->getUAVMipIndex(0) }, 0);
		context->transitionResources();
		context->dispatch(pressureTex->width / 16, pressureTex->height / 9, 1);
		context->uavBarrier({ pressureTex->write()->getTexture() });
		pressureTex->swap();

		//calculate pressure
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

		//velocity subtract gradient of pressure
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
	}

	void advect()
	{
		//velocity advection
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

		//color advection
		context->setPipelineState(colorAdvectionState.Get());
		context->setCSConstants({ velocityTex->read()->getAllSRVIndex(),colorTex->read()->getAllSRVIndex(),colorTex->write()->getUAVMipIndex(0) }, 0);
		context->transitionResources();
		context->dispatch(colorTex->width / 16, colorTex->height / 9, 1);
		context->uavBarrier({ colorTex->write()->getTexture() });
		colorTex->swap();
	}

	void recordCommand() override
	{
		const DirectX::XMFLOAT2 pos =
		{
			(float)Mouse::getX() / Graphics::getWidth(),
			(float)(Graphics::getHeight() - Mouse::getY()) / Graphics::getHeight()
		};

		const DirectX::XMFLOAT2 posDelta =
		{
			(pos.x - simulationParam.pos.x) * config.splatForce,
			((pos.y - simulationParam.pos.y) / Graphics::getAspectRatio()) * config.splatForce
		};

		simulationParam.pos = pos;
		simulationParam.posDelta = posDelta;

		//simulationParam.pos = DirectX::XMFLOAT2(0.10, 0.5);
		//simulationParam.posDelta = DirectX::XMFLOAT2(15.0, 0.0);

		if (colorUpdateTimer.update(Graphics::getDeltaTime() * config.colorChangeSpeed))
		{
			Color c = Color::HSVtoRGB({ Random::Float(),1.f,1.f });
			simulationParam.splatColor = { c.r,c.g,c.b,1.f };
		}

		simulationParamBuffer->update(&simulationParam, sizeof(SimulationParam));

		context->setGlobalConstantBuffer(simulationParamBuffer);

		splatVelocityAndColor();

		vorticityConfinement();

		project();

		advect();

		context->setTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
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

	TextureRenderView* vorticityTex;

	TextureRenderView* divergenceTex;

	SwapTexture* pressureTex;

	TextureRenderView* originTexture;

	struct Configuration
	{
		float colorChangeSpeed = 10.f;//颜色改变速度
		float colorDissipationSpeed = 1.f;//颜色消散速度
		float velocityDissipationSpeed = 0.00f;//速度消散速度
		float vorticityIntensity = 80.f;//涡流强度
		float splatRadius = 0.25f;//施加颜色的半径
		float splatForce = 6000.f;//施加速度的强度
		const unsigned int pressureIteraion = 35;//雅可比迭代次数 这个值越高物理模拟越不容易出错 NVIDIA的文章有提到通常20-50次就够了
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
		float vorticityIntensity;
		float splatRadius;
		DirectX::XMFLOAT4 padding[11];
	} simulationParam;

	ConstantBuffer* simulationParamBuffer;

	Timer colorUpdateTimer;

	Shader* splatVelocityCS;

	ComPtr<ID3D12PipelineState> splatVelocityState;

	Shader* splatColorCS;

	ComPtr<ID3D12PipelineState> splatColorState;

	Shader* vorticityCS;

	ComPtr<ID3D12PipelineState> vorticityState;

	Shader* vorticityConfinementCS;

	ComPtr<ID3D12PipelineState> vorticityConfinementState;

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