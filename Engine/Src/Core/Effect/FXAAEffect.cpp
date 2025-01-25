#include<Gear/Core/Effect/FXAAEffect.h>

#include<Gear/CompiledShaders/ColorToColorLuma.h>

#include<Gear/CompiledShaders/FXAA.h>

FXAAEffect::FXAAEffect(GraphicsContext* const context, const uint32_t width, const uint32_t height) :
	Effect(context, width, height, DXGI_FORMAT_R8G8B8A8_UNORM), fxaaParam{ 1.0f,0.75f,0.166f,0.0633f },
	colorLumaTexture(ResourceManager::createTextureRenderView(width, height, DXGI_FORMAT_R8G8B8A8_UNORM, 1, 1, false, true,
		DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_R8G8B8A8_UNORM))
{
	colorToColorLumaPS = new Shader(g_ColorToColorLumaBytes, sizeof(g_ColorToColorLumaBytes));

	fxaaPS = new Shader(g_FXAABytes, sizeof(g_FXAABytes));

	{
		D3D12_GRAPHICS_PIPELINE_STATE_DESC desc = PipelineState::getDefaultFullScreenState();
		desc.PS = colorToColorLumaPS->getByteCode();
		desc.NumRenderTargets = 1;
		desc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;

		GraphicsDevice::get()->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(&colorToColorLumaState));
	}

	{
		D3D12_GRAPHICS_PIPELINE_STATE_DESC desc = PipelineState::getDefaultFullScreenState();
		desc.PS = fxaaPS->getByteCode();
		desc.NumRenderTargets = 1;
		desc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;

		GraphicsDevice::get()->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(&fxaaState));
	}
}

FXAAEffect::~FXAAEffect()
{
	delete colorLumaTexture;
	delete colorToColorLumaPS;
	delete fxaaPS;
}

TextureRenderView* FXAAEffect::process(TextureRenderView* const inputTexture) const
{
	context->setPipelineState(colorToColorLumaState.Get());

	context->setViewportSimple(width, height);

	context->setTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	context->setRenderTargets({ colorLumaTexture->getRTVMipHandle(0) });

	context->setPSConstants({ inputTexture->getAllSRVIndex() }, 0);

	context->transitionResources();

	context->draw(3, 1, 0, 0);

	context->setPipelineState(fxaaState.Get());

	context->setViewportSimple(width, height);

	context->setTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	context->setRenderTargets({ outputTexture->getRTVMipHandle(0) });

	context->setPSConstants({ colorLumaTexture->getAllSRVIndex() }, 0);

	context->setPSConstants(4, &fxaaParam, 1);

	context->transitionResources();

	context->draw(3, 1, 0, 0);

	return outputTexture;
}

void FXAAEffect::imGUICall()
{
	ImGui::Begin("FXAA Effect");
	ImGui::SliderFloat("FXAAQualitySubpix", &fxaaParam.fxaaQualitySubpix, 0.0f, 1.f);
	ImGui::SliderFloat("FXAAQualityEdgeThreshold", &fxaaParam.fxaaQualityEdgeThreshold, 0.0f, 1.f);
	ImGui::SliderFloat("FXAAQualityEdgeThresholdMin", &fxaaParam.fxaaQualityEdgeThresholdMin, 0.0f, 1.f);
	ImGui::End();
}

void FXAAEffect::setFXAAQualitySubpix(const float fxaaQualitySubpix)
{
	fxaaParam.fxaaQualitySubpix = fxaaQualitySubpix;
}

void FXAAEffect::setFXAAQualityEdgeThreshold(const float fxaaQualityEdgeThreshold)
{
	fxaaParam.fxaaQualityEdgeThreshold = fxaaQualityEdgeThreshold;
}

void FXAAEffect::setFXAAQualityEdgeThresholdMin(const float fxaaQualityEdgeThresholdMin)
{
	fxaaParam.fxaaQualityEdgeThresholdMin = fxaaQualityEdgeThresholdMin;
}
