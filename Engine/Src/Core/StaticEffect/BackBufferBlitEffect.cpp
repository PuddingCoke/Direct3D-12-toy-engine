#include<Gear/Core/StaticEffect/BackBufferBlitEffect.h>

#include<Gear/Core/StaticEffect/Internal/BackBufferBlitEffectInternal.h>

#include<Gear/Core/Graphics.h>

namespace
{
	struct BackBufferBlitEffectPrivate
	{
		ComPtr<ID3D12PipelineState> backBufferBlitState;
	}pvt;
}

void Core::StaticEffect::BackBufferBlitEffect::process(GraphicsContext* const context, TextureRenderView* const inputTexture)
{
	context->setPipelineState(pvt.backBufferBlitState.Get());

	context->setDefRenderTarget();

	context->setViewportSimple(Core::Graphics::getWidth(), Core::Graphics::getHeight());

	context->setTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	context->setPSConstants({ inputTexture->getAllSRVIndex() }, 0);

	context->transitionResources();

	context->draw(3, 1, 0, 0);
}

void Core::StaticEffect::BackBufferBlitEffect::Internal::initialize()
{
	D3D12_GRAPHICS_PIPELINE_STATE_DESC desc = Core::PipelineStateHelper::getDefaultFullScreenState();
	desc.PS = Core::GlobalShader::getFullScreenPS()->getByteCode();
	desc.NumRenderTargets = 1;
	desc.RTVFormats[0] = Core::Graphics::backBufferFormat;

	Core::GraphicsDevice::get()->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(&pvt.backBufferBlitState));

	LOGSUCCESS(L"create", LogColor::brightMagenta, L"BackBufferBlitEffect", LogColor::defaultColor, L"succeeded");
}

void Core::StaticEffect::BackBufferBlitEffect::Internal::release()
{
	pvt.backBufferBlitState = nullptr;
}
