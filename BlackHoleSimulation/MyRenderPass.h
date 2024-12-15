#pragma once

#include<Gear/Core/RenderPass.h>
#include<Gear/Core/Shader.h>
#include<Gear/Core/Effect/BloomEffect.h>

#include<Gear/Core/Resource/TextureRenderView.h>

class MyRenderPass :public RenderPass
{
public:

	MyRenderPass() :
		blackHoleAccumulateCS(new Shader(Utils::getRootFolder() + "BlackHoleAccumulateCS.cso"))
	{
		accumulateTexture = ResourceManager::createTextureRenderView(Graphics::getWidth(), Graphics::getHeight(), DXGI_FORMAT_R16G16B16A16_FLOAT, 1, 1, false, true,
			DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_UNKNOWN);

		PipelineState::createComputeState(&blackHoleAccumulateState, blackHoleAccumulateCS);

		Mouse::addMoveEvent([this]()
			{
				if (Mouse::getLeftDown())
				{
					renderParam.phi -= Mouse::getDY() * Graphics::getDeltaTime();
					renderParam.theta -= Mouse::getDX() * Graphics::getDeltaTime();
					renderParam.phi = Math::clamp(renderParam.phi, -Math::half_pi + 0.01f, Math::half_pi - 0.01f);

					renderParam.frameIndex = 0;
				}
			});

		envCube = resManager->createTextureCube("E:\\Assets\\SpaceShip\\360-Space-Panorama-III.hdr", 2048, true);

		noiseTexture = resManager->createTextureRenderView(512, 512, RandomDataType::NOISE, true);

		diskTexture = resManager->createTextureRenderView("DiskTexture.jpg", true);

		effect = new BloomEffect(context, Graphics::getWidth(), Graphics::getHeight(), resManager);

		effect->setExposure(1.6f);

		effect->setGamma(2.2f);

	}

	~MyRenderPass()
	{
		delete accumulateTexture;

		delete envCube;

		delete noiseTexture;

		delete diskTexture;

		delete effect;

		delete blackHoleAccumulateCS;
	}

	void imGUICall() override
	{
		effect->imGUICall();

		ImGui::Begin("Render Parameters");
		ImGui::SliderFloat("radius", &renderParam.radius, 8.f, 12.f);
		ImGui::End();
	}

protected:

	void recordCommand() override
	{
		renderParam.frameIndex++;

		context->setPipelineState(blackHoleAccumulateState.Get());;

		context->setCSConstants({ accumulateTexture->getUAVMipIndex(0),envCube->getAllSRVIndex(),noiseTexture->getAllSRVIndex(),diskTexture->getAllSRVIndex() }, 0);

		context->setCSConstants(sizeof(RenderParam) / 4, &renderParam, 4);

		context->transitionResources();

		context->dispatch(Graphics::getWidth() / 16, Graphics::getHeight() / 9, 1);

		context->uavBarrier({ accumulateTexture->getTexture() });

		auto tex = effect->process(accumulateTexture);

		blit(tex);
	}

private:

	TextureRenderView* accumulateTexture;

	TextureRenderView* envCube;

	TextureRenderView* noiseTexture;

	TextureRenderView* diskTexture;

	BloomEffect* effect;

	ComPtr<ID3D12PipelineState> blackHoleAccumulateState;

	Shader* blackHoleAccumulateCS;

	struct RenderParam
	{
		UINT frameIndex = 0;
		float phi = 0.f;
		float theta = 0.f;
		float radius = 10.f;
	} renderParam;

};