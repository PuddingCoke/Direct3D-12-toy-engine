#pragma once

#include<Gear/Core/RenderTask.h>

#include<Gear/Core/Effect/BloomEffect.h>

#include<Gear/DevEssential.h>

class MyRenderTask :public RenderTask
{
public:

	MyRenderTask() :
		colorUpdateTimer(1.f),
		splatVelocityCS(new Shader(Utils::File::getRootFolder() + L"SplatVelocityCS.cso")),
		splatColorCS(new Shader(Utils::File::getRootFolder() + L"SplatColorCS.cso")),
		vorticityCS(new Shader(Utils::File::getRootFolder() + L"VorticityCS.cso")),
		vorticityConfinementCS(new Shader(Utils::File::getRootFolder() + L"VorticityConfinementCS.cso")),
		divergenceCS(new Shader(Utils::File::getRootFolder() + L"DivergenceCS.cso")),
		pressureResetCS(new Shader(Utils::File::getRootFolder() + L"PressureResetCS.cso")),
		pressureCS(new Shader(Utils::File::getRootFolder() + L"PressureCS.cso")),
		gradientSubtractCS(new Shader(Utils::File::getRootFolder() + L"GradientSubtractCS.cso")),
		velocityAdvectionCS(new Shader(Utils::File::getRootFolder() + L"VelocityAdvectionCS.cso")),
		colorAdvectionCS(new Shader(Utils::File::getRootFolder() + L"ColorAdvectionCS.cso")),
		velocityBoundaryCS(new Shader(Utils::File::getRootFolder() + L"VelocityBoundaryCS.cso")),
		pressureBoundaryCS(new Shader(Utils::File::getRootFolder() + L"PressureBoundaryCS.cso")),
		phongShadeCS(new Shader(Utils::File::getRootFolder() + L"PhongShadeCS.cso")),
		edgeHighlightCS(new Shader(Utils::File::getRootFolder() + L"EdgeHighlightCS.cso"))
	{
		const DirectX::XMUINT2 simRes = { Graphics::getWidth() >> config.resolutionFactor,Graphics::getHeight() >> config.resolutionFactor };

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

		phongShadeTexture = ResourceManager::createTextureRenderView(Graphics::getWidth(), Graphics::getHeight(), DXGI_FORMAT_R16G16B16A16_FLOAT, 1, 1, false, true,
			DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_UNKNOWN);

		edgeHighlightTexture = ResourceManager::createTextureRenderView(Graphics::getWidth(), Graphics::getHeight(), DXGI_FORMAT_R8G8B8A8_UNORM, 1, 1, false, true,
			DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_UNKNOWN);

		simulationParamBuffer = ResourceManager::createDynamicCBuffer(sizeof(SimulationParam));

		simulationParam.colorTexelSize = DirectX::XMFLOAT2(1.f / Graphics::getWidth(), 1.f / Graphics::getHeight());

		simulationParam.simTexelSize = DirectX::XMFLOAT2(1.f / simRes.x, 1.f / simRes.y);

		simulationParam.colorTextureSize = DirectX::XMUINT2(Graphics::getWidth(), Graphics::getHeight());

		simulationParam.simTextureSize = DirectX::XMUINT2(simRes.x, simRes.y);

		simulationParam.colorDissipationSpeed = config.colorDissipationSpeed;

		simulationParam.velocityDissipationSpeed = config.velocityDissipationSpeed;

		simulationParam.vorticityIntensity = config.vorticityIntensity;

		PipelineStateHelper::createComputeState(&splatVelocityState, splatVelocityCS);

		PipelineStateHelper::createComputeState(&splatColorState, splatColorCS);

		PipelineStateHelper::createComputeState(&vorticityState, vorticityCS);

		PipelineStateHelper::createComputeState(&vorticityConfinementState, vorticityConfinementCS);

		PipelineStateHelper::createComputeState(&divergenceState, divergenceCS);

		PipelineStateHelper::createComputeState(&pressureResetState, pressureResetCS);

		PipelineStateHelper::createComputeState(&pressureState, pressureCS);

		PipelineStateHelper::createComputeState(&gradientSubtractState, gradientSubtractCS);

		PipelineStateHelper::createComputeState(&velocityAdvectionState, velocityAdvectionCS);

		PipelineStateHelper::createComputeState(&colorAdvectionState, colorAdvectionCS);

		PipelineStateHelper::createComputeState(&velocityBoundaryState, velocityBoundaryCS);

		PipelineStateHelper::createComputeState(&pressureBoundaryState, pressureBoundaryCS);

		PipelineStateHelper::createComputeState(&phongShadeState, phongShadeCS);

		PipelineStateHelper::createComputeState(&edgeHighlightState, edgeHighlightCS);

		effect = new BloomEffect(context, Graphics::getWidth(), Graphics::getHeight(), resManager);

		effect->setThreshold(0.f);

		kDownEventID = Input::Keyboard::addKeyDownEvent(Input::Keyboard::K, [this]()
			{
				config.logicRunning = !config.logicRunning;
			});

	}

	~MyRenderTask()
	{
		Input::Keyboard::removeKeyDownEvent(Input::Keyboard::K, kDownEventID);


		delete effect;

		delete colorTex;
		delete velocityTex;
		delete vorticityTex;
		delete divergenceTex;
		delete pressureTex;

		delete phongShadeTexture;
		delete edgeHighlightTexture;

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
		delete phongShadeCS;
		delete edgeHighlightCS;
	}

	void imGUICall() override
	{
		ImGui::Begin("Parameters");
		ImGui::SliderFloat("color change speed", &config.colorChangeSpeed, 0.f, 50.f);
		ImGui::SliderFloat("color dissipation speed", &simulationParam.colorDissipationSpeed, 0.f, 10.f);
		ImGui::SliderFloat("velocity dissipation speed", &simulationParam.velocityDissipationSpeed, 0.f, 1.f);
		ImGui::SliderFloat("vorticity intensity", &simulationParam.vorticityIntensity, 0.f, 100.f);
		ImGui::SliderFloat("radius", &config.splatRadius, 0.f, 1.0f);
		ImGui::SliderFloat("force", &config.force, 0.f, 10.f);
		ImGui::SliderFloat("kA", &simulationParam.kA, 0.f, 2.f);
		ImGui::SliderFloat("kD", &simulationParam.kD, 0.f, 2.f);
		ImGui::SliderFloat("bumpScale", &config.bumpScale, 100.f, 500.f);
		ImGui::SliderFloat("edgeMagnitudeScale", &simulationParam.edgeMagnitudeScale, 0.f, 6.f);
		ImGui::Checkbox("logic running", &config.logicRunning);
		ImGui::Checkbox("phong shade", &config.phongShading);
		ImGui::Checkbox("edge detection & highlight", &config.edgeHighlight);
		ImGui::Checkbox("vortex", &config.vortex);
		ImGui::End();

		effect->imGUICall();
	}

	void splatVelocityAndColor()
	{
		if (config.vortex || (Input::Mouse::onMove() && Input::Mouse::getLeftDown()))
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
		const DirectX::XMFLOAT2 pos = { (float)Input::Mouse::getX(),(float)(Graphics::getHeight() - Input::Mouse::getY()) };

		const DirectX::XMFLOAT2 posDelta = { (pos.x - simulationParam.pos.x) * config.force,(pos.y - simulationParam.pos.y) * config.force };

		if (config.vortex)
		{
			simulationParam.pos = DirectX::XMFLOAT2(200.f, Graphics::getHeight() / 2.f);

			simulationParam.splatColor.x *= Graphics::getDeltaTime() * 144.f;

			simulationParam.splatColor.y *= Graphics::getDeltaTime() * 144.f;

			simulationParam.splatColor.z *= Graphics::getDeltaTime() * 144.f;

			simulationParam.posDelta = DirectX::XMFLOAT2(7200.0f * Graphics::getDeltaTime(), 0.0f);
		}
		else
		{
			simulationParam.pos = pos;

			simulationParam.posDelta = posDelta;
		}

		if (colorUpdateTimer.update(Graphics::getDeltaTime() * config.colorChangeSpeed))
		{
			Utils::Color c = Utils::Color::hsvToRgb({ Utils::Random::genFloat(),1.f,1.f });

			simulationParam.splatColor = { c.r,c.g,c.b,1.f };
		}

		simulationParam.splatRadius = config.splatRadius / 50.f;

		simulationParam.bumpScale = 1.f / config.bumpScale;

		simulationParamBuffer->simpleUpdate(&simulationParam);

		context->setGlobalConstantBuffer(simulationParamBuffer);

		if (config.logicRunning)
		{
			splatVelocityAndColor();

			vorticityConfinement();

			project();

			advect();
		}

		TextureRenderView* outputTexture = nullptr;

		if (config.phongShading)
		{
			context->setPipelineState(phongShadeState.Get());
			context->setCSConstants({ colorTex->read()->getAllSRVIndex(),phongShadeTexture->getUAVMipIndex(0) }, 0);
			context->transitionResources();
			context->dispatch(phongShadeTexture->getTexture()->getWidth() / 16, phongShadeTexture->getTexture()->getHeight() / 9, 1);
			context->uavBarrier({ phongShadeTexture->getTexture() });

			outputTexture = effect->process(phongShadeTexture);
		}
		else
		{
			outputTexture = effect->process(colorTex->read());
		}

		if (config.edgeHighlight)
		{
			context->setPipelineState(edgeHighlightState.Get());
			context->setCSConstants({ outputTexture->getAllSRVIndex(),edgeHighlightTexture->getUAVMipIndex(0) }, 0);
			context->transitionResources();
			context->dispatch(edgeHighlightTexture->getTexture()->getWidth() / 16, edgeHighlightTexture->getTexture()->getHeight() / 9, 1);
			context->uavBarrier({ edgeHighlightTexture->getTexture() });

			blit(edgeHighlightTexture);
		}
		else
		{
			blit(outputTexture);
		}
	}

private:

	BloomEffect* effect;

	SwapTexture* colorTex;

	SwapTexture* velocityTex;

	TextureRenderView* vorticityTex;

	TextureRenderView* divergenceTex;

	SwapTexture* pressureTex;

	TextureRenderView* phongShadeTexture;

	TextureRenderView* edgeHighlightTexture;

	struct Configuration
	{
		float colorChangeSpeed = 10.f;//颜色改变速度
		float colorDissipationSpeed = 1.f;//颜色消散速度
		float velocityDissipationSpeed = 0.00f;//速度消散速度
		float vorticityIntensity = 80.f;//涡流强度
		float splatRadius = 0.25f;//施加颜色的半径
		float force = 6.f;//施加速度的力度
		const unsigned int pressureIteraion = 35;//雅可比迭代次数 这个值越高物理模拟越不容易出错 NVIDIA的文章有提到通常20-50次就够了
		const unsigned int resolutionFactor = 1;//物理模拟分辨率 resolution >> resolutionFactor
		float bumpScale = 300.f;
		bool logicRunning = true;
		bool phongShading = true;
		bool edgeHighlight = true;
		bool vortex = false;
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
		float kA = 0.6f;
		float kD = 0.4f;
		float bumpScale = 1.f / 300.f;
		float edgeMagnitudeScale = 1.5f;
		DirectX::XMFLOAT4 padding1[10] = {};
	} simulationParam;

	int kDownEventID;

	DynamicCBuffer* simulationParamBuffer;

	Utils::Timer colorUpdateTimer;

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

	Shader* phongShadeCS;

	ComPtr<ID3D12PipelineState> phongShadeState;

	Shader* edgeHighlightCS;

	ComPtr<ID3D12PipelineState> edgeHighlightState;

};