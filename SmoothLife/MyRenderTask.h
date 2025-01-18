#pragma once

#include<Gear/Core/RenderTask.h>
#include<Gear/Core/Shader.h>

#include<Gear/Core/Resource/TextureRenderView.h>

class MyRenderTask :public RenderTask
{
public:

	MyRenderTask() :
		timer(1.f / 60.f),
		whiteNoiseCS(new Shader(Utils::getRootFolder() + "WhiteNoiseCS.cso")),
		evolveCS(new Shader(Utils::getRootFolder() + "EvolveCS.cso")),
		visualizeCS(new Shader(Utils::getRootFolder() + "VisualizeCS.cso")),
		originTexture(ResourceManager::createTextureRenderView(Graphics::getWidth(), Graphics::getHeight(), DXGI_FORMAT_R8G8B8A8_UNORM, 1, 1, false, true,
			DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_UNKNOWN))
	{
		PipelineState::createComputeState(&whiteNoiseState, whiteNoiseCS);

		PipelineState::createComputeState(&evolveState, evolveCS);

		PipelineState::createComputeState(&visualizeState, visualizeCS);

		swapTexture = new SwapTexture([] {return ResourceManager::createTextureRenderView(Graphics::getWidth(), Graphics::getHeight(), DXGI_FORMAT_R32_FLOAT, 1, 1, false, true,
			DXGI_FORMAT_R32_FLOAT, DXGI_FORMAT_R32_FLOAT, DXGI_FORMAT_UNKNOWN); });

		initialize();
	}

	~MyRenderTask()
	{
		delete whiteNoiseCS;
		delete evolveCS;
		delete visualizeCS;
		delete swapTexture;
		delete originTexture;
	}

	/*
		float ra = 12.f;
		float b1 = 0.278f;
		float d1 = 0.267f;
		float b2 = 0.365f;
		float d2 = 0.445f;
		float alpha_n = 0.028f;
		float alpha_m = 0.147f;
	*/
	void imGUICall() override
	{
		ImGui::Begin("Simulation Parameters");
		ImGui::SliderFloat("ra", &simulationParam.ra, 0.f, 16.f);
		ImGui::SliderFloat("b1", &simulationParam.b1, 0.f, 1.f);
		ImGui::SliderFloat("d1", &simulationParam.d1, 0.f, 1.f);
		ImGui::SliderFloat("b2", &simulationParam.b2, 0.f, 1.f);
		ImGui::SliderFloat("d2", &simulationParam.d2, 0.f, 1.f);
		ImGui::SliderFloat("alpha_n", &simulationParam.alpha_n, 0.f, 1.f);
		ImGui::SliderFloat("alpha_m", &simulationParam.alpha_m, 0.f, 1.f);
		ImGui::End();
	}

protected:

	void whiteNoise(SwapTexture* swapTexture)
	{
		context->setPipelineState(whiteNoiseState.Get());

		context->setCSConstants({ swapTexture->write()->getUAVMipIndex(0) }, 0);

		UINT uintSeed = Random::genUint();

		context->setCSConstants(1, &uintSeed, 1);

		context->transitionResources();

		context->dispatch(swapTexture->width / 16, swapTexture->height / 9, 1);

		context->uavBarrier({ swapTexture->write()->getTexture() });

		swapTexture->swap();
	}

	void evolve(SwapTexture* swapTexture)
	{
		context->setPipelineState(evolveState.Get());

		context->setCSConstants({ swapTexture->read()->getAllSRVIndex(),
			swapTexture->write()->getUAVMipIndex(0) }, 0);

		context->setCSConstants(8, &simulationParam, 2);

		context->transitionResources();

		context->dispatch(swapTexture->width / 16, swapTexture->height / 9, 1);

		context->uavBarrier({ swapTexture->write()->getTexture() });

		swapTexture->swap();
	}

	void initialize()
	{
		whiteNoise(swapTexture);
	}

	void step()
	{
		while (timer.update(Graphics::getDeltaTime()))
		{
			evolve(swapTexture);
		}
	}

	void visualize()
	{
		context->setPipelineState(visualizeState.Get());

		context->setCSConstants({ originTexture->getUAVMipIndex(0),
			swapTexture->read()->getAllSRVIndex() }, 0);

		context->transitionResources();

		context->dispatch(swapTexture->width / 16, swapTexture->height / 9, 1);

		context->uavBarrier({ originTexture->getTexture() });

		blit(originTexture);
	}

	void recordCommand() override
	{
		simulationParam.ri = simulationParam.ra / 3.f;

		if (Keyboard::onKeyDown(Keyboard::K))
		{
			whiteNoise(swapTexture);
		}

		step();

		visualize();
	}

private:

	struct SimulationParam
	{
		float ra = 12.f;
		float ri = 4.f;
		float b1 = 0.278f;
		float d1 = 0.267f;
		float b2 = 0.365f;
		float d2 = 0.445f;
		float alpha_n = 0.028f;
		float alpha_m = 0.147f;
	}simulationParam;

	Shader* whiteNoiseCS;

	ComPtr<ID3D12PipelineState> whiteNoiseState;

	Shader* evolveCS;

	ComPtr<ID3D12PipelineState> evolveState;

	Shader* visualizeCS;

	ComPtr<ID3D12PipelineState> visualizeState;

	SwapTexture* swapTexture;

	TextureRenderView* originTexture;

	Timer timer;

};