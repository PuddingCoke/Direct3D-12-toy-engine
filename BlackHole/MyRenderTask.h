#pragma once

#include<Gear/Core/RenderTask.h>
#include<Gear/Core/Shader.h>

#include<Gear/Core/Resource/TextureRenderView.h>

#include<Gear/Core/Effect/BloomEffect.h>

class MyRenderTask :public RenderTask
{
public:

	MyRenderTask() :
		blackHoleShader(new Shader(Utils::getRootFolder() + L"BlackHolePS.cso")),
		noiseTexture(resManager->createTextureRenderView(L"Noise.png", true)),
		diskTexture(resManager->createTextureRenderView(L"Disk.jpg", true)),
		originTexture(ResourceManager::createTextureRenderView(Graphics::getWidth(), Graphics::getHeight(), DXGI_FORMAT_R16G16B16A16_FLOAT, 1, 1, false, true,
			DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_R16G16B16A16_FLOAT)),
		effect(new BloomEffect(context, Graphics::getWidth(), Graphics::getHeight(), resManager))
	{
		auto desc = PipelineState::getDefaultFullScreenState();
		desc.NumRenderTargets = 1;
		desc.RTVFormats[0] = originTexture->getTexture()->getFormat();
		desc.PS = blackHoleShader->getByteCode();

		GraphicsDevice::get()->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(&pipelineState));
	}

	~MyRenderTask()
	{
		delete blackHoleShader;

		delete noiseTexture;

		delete diskTexture;

		delete originTexture;

		delete effect;
	}

	void imGUICall() override
	{
		effect->imGUICall();

		ImGui::Begin("Black Hole Parameters");
		ImGui::SliderFloat("Period", &perframeData.period, 60.f, 180.f);
		ImGui::End();
	}

protected:

	void recordCommand() override
	{
		context->setRenderTargets({ originTexture->getRTVMipHandle(0) });

		context->setPipelineState(pipelineState.Get());

		context->setViewportSimple(Graphics::getWidth(), Graphics::getHeight());

		context->setTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		context->setPSConstants({ noiseTexture->getAllSRVIndex(),diskTexture->getAllSRVIndex() }, 0);

		perframeData.width = static_cast<float>(Graphics::getWidth());
		perframeData.height = static_cast<float>(Graphics::getHeight());
		perframeData.timeElapsed = Graphics::getTimeElapsed();

		context->setPSConstants(sizeof(PerframeData) / 4ull, &perframeData, 2);

		context->transitionResources();

		context->draw(3, 1, 0, 0);

		auto outputTexture = effect->process(originTexture);

		blit(outputTexture);
	}

private:

	ComPtr<ID3D12PipelineState> pipelineState;

	Shader* blackHoleShader;

	TextureRenderView* noiseTexture;

	TextureRenderView* diskTexture;

	TextureRenderView* originTexture;

	BloomEffect* effect;

	struct PerframeData
	{
		float width;
		float height;
		float timeElapsed;
		float period = 120.f;
	} perframeData;

};