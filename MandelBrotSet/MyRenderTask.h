#pragma once

#include<Gear/Core/RenderTask.h>
#include<Gear/Core/Shader.h>

#include<Gear/Core/Resource/TextureRenderView.h>

#include<Gear/Core/Effect/BloomEffect.h>

class MyRenderTask :public RenderTask
{
public:

	MyRenderTask() :
		effect(context, Graphics::getWidth(), Graphics::getHeight(), resManager),
		computeCS(new Shader(Utils::getRootFolder() + L"ComputeCS.cso")),
		originTexture(ResourceManager::createTextureRenderView(Graphics::getWidth(), Graphics::getHeight(), DXGI_FORMAT_R16G16B16A16_UNORM, 1, 1, false, true,
			DXGI_FORMAT_R16G16B16A16_UNORM, DXGI_FORMAT_R16G16B16A16_UNORM, DXGI_FORMAT_UNKNOWN))
	{
		PipelineState::createComputeState(&computeState, computeCS);

		effect.setExposure(1.9f);

		effect.setGamma(1.2f);

		effect.setIntensity(1.f);

		param.scale = 0.4f;

		param.lerpFactor = 0.4f;

		param.lerpFactor2 = 0.7f;
	}

	~MyRenderTask()
	{
		delete computeCS;

		delete originTexture;
	}

	void imGUICall() override
	{
		ImGui::Begin("Simulation Parameters");
		ImGui::SliderFloat("scale", &param.scale, 0.f, 1.f);
		ImGui::SliderFloat("lerpFactor", &param.lerpFactor, 0.f, 1.f);
		ImGui::SliderFloat("lerpFactor2", &param.lerpFactor2, 0.f, 1.f);
		ImGui::End();

		effect.imGUICall();
	}

protected:

	void recordCommand() override
	{
		if (Mouse::onScroll())
		{
			if (Mouse::getWheelDelta() > 0.f)
			{
				param.scale *= 0.85f;
			}
			else
			{
				param.scale /= 0.85f;
			}

			accParam.frameIndex = 0;
		}

		if (Mouse::getLeftDown() && Mouse::onMove())
		{
			param.location.x -= Graphics::getDeltaTime() * Mouse::getDX() * param.scale;

			param.location.y += Graphics::getDeltaTime() * Mouse::getDY() * param.scale;

			accParam.frameIndex = 0;
		}

		accParam.frameIndex++;

		accParam.floatSeed = Graphics::getTimeElapsed();

		context->setPipelineState(computeState.Get());

		context->setCSConstants({ originTexture->getUAVMipIndex(0) }, 0);

		context->setCSConstants(sizeof(SimulationParam) / 4, &param, 1);

		context->setCSConstants(2, &accParam, sizeof(SimulationParam) / 4 + 1);

		context->transitionResources();

		context->dispatch(Graphics::getWidth() / 16, Graphics::getHeight() / 9, 1);

		context->uavBarrier({ originTexture->getTexture() });

		auto bloomTex = effect.process(originTexture);

		blit(bloomTex);
	}

private:

	Shader* computeCS;

	ComPtr<ID3D12PipelineState> computeState;

	TextureRenderView* originTexture;

	struct SimulationParam
	{
		DirectX::XMFLOAT2 location = { 0.f,0.f };
		float scale = { 1.f };
		const DirectX::XMFLOAT2 texelSize = { 1.f / static_cast<float>(Graphics::getWidth()),1.f / static_cast<float>(Graphics::getHeight()) };
		float lerpFactor = 0.f;
		float lerpFactor2 = 0.f;
	} param;

	struct AccumulateParam
	{
		UINT frameIndex;
		float floatSeed;
	} accParam;

	BloomEffect effect;

};