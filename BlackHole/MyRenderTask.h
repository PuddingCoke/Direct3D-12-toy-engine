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

		Keyboard::addKeyDownEvent(Keyboard::K, [this]() {perframeData.useOriginalVer = ~perframeData.useOriginalVer; });
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
		ImGui::SliderFloat("Period", &perframeData.period, 10.f, 180.f);
		ImGui::Checkbox("moveP", (bool*)&perframeData.moveP);
		ImGui::SliderFloat("GEXP", &perframeData.G, -1.f, 0.05f);
		ImGui::Checkbox("Use Original Ver", (bool*)&perframeData.useOriginalVer);
		ImGui::SliderFloat("Exponent1", &perframeData.exponent1, 0.0f, 2.0f);
		ImGui::SliderFloat("Scale1", &perframeData.scale1, 0, 2.f);
		ImGui::SliderFloat("Bias1", &perframeData.bias1, -1.f, 1.f);
		ImGui::SliderFloat("Exponent2", &perframeData.exponent2, 0.0f, 2.0f);
		ImGui::SliderFloat("Scale2", &perframeData.scale2, 0, 2.f);
		ImGui::SliderFloat("Bias2", &perframeData.bias2, -1.f, 1.f);
		ImGui::SliderFloat("BaseNoise2ScaleFactor", &perframeData.baseNoise2ScaleFactor, 0.f, 1.f);
		ImGui::SliderFloat("Noise2LerpFactor", &perframeData.noise2LerpFactor, 0.f, 1.f);
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
		uint32_t moveP = true;
		float G = 0.f;
		uint32_t useOriginalVer = false;
		float exponent1 = 0.6f;
		float scale1 = 1.3f;
		float bias1 = 0.15f;
		float exponent2 = 0.7f;
		float scale2 = 1.2f;
		float bias2 = 0.1f;
		float baseNoise2ScaleFactor = 1.f;
		float noise2LerpFactor = 0.7f;
	} perframeData;

};