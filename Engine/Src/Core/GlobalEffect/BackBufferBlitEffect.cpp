#include<Gear/Core/GlobalEffect/BackBufferBlitEffect.h>

#include<Gear/Core/GlobalEffect/Internal/BackBufferBlitEffectInternal.h>

#include<Gear/Core/GlobalShader.h>

#include<Gear/Core/Graphics.h>

namespace
{
	struct BackBufferBlitEffectPrivate
	{
		ComPtr<ID3D12PipelineState> backBufferBlitState;
	}pvt;
}

void Gear::Core::GlobalEffect::BackBufferBlitEffect::process(GraphicsContext* const context, Resource::TextureRenderView* const inputTexture)
{
	context->setPipelineState(pvt.backBufferBlitState.Get());

	context->setDefRenderTarget();

	context->setViewportSimple(Gear::Core::Graphics::getWidth(), Gear::Core::Graphics::getHeight());

	context->setTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	context->setPSConstants({ inputTexture->getAllSRVIndex() }, 0);

	context->transitionResources();

	context->draw(3, 1, 0, 0);
}

void Gear::Core::GlobalEffect::BackBufferBlitEffect::Internal::initialize()
{
	D3D12_GRAPHICS_PIPELINE_STATE_DESC desc = Gear::Core::PipelineStateHelper::getDefaultFullScreenState();
	desc.PS = Gear::Core::GlobalShader::getFullScreenPS()->getByteCode();
	desc.NumRenderTargets = 1;
	desc.RTVFormats[0] = Gear::Core::Graphics::backBufferFormat;

	Gear::Core::GraphicsDevice::get()->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(&pvt.backBufferBlitState));

	LOGSUCCESS(L"create", LogColor::brightMagenta, L"BackBufferBlitEffect", LogColor::defaultColor, L"succeeded");
}

void Gear::Core::GlobalEffect::BackBufferBlitEffect::Internal::release()
{
	pvt.backBufferBlitState = nullptr;
}
