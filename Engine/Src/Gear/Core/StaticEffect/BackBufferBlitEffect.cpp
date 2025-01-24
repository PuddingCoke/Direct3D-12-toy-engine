#include<Gear/Core/StaticEffect/BackBufferBlitEffect.h>

#include<Gear/Core/PipelineState.h>

#include<Gear/Core/States.h>

BackBufferBlitEffect* BackBufferBlitEffect::instance = nullptr;

BackBufferBlitEffect* BackBufferBlitEffect::get()
{
	return instance;
}

void BackBufferBlitEffect::process(GraphicsContext* const context, TextureRenderView* const inputTexture)
{
	context->setPipelineState(backBufferBlitState.Get());

	context->setDefRenderTarget();

	context->setViewportSimple(Graphics::getWidth(), Graphics::getHeight());

	context->setTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	context->setPSConstants({ inputTexture->getAllSRVIndex() }, 0);

	context->transitionResources();

	context->draw(3, 1, 0, 0);
}

BackBufferBlitEffect::BackBufferBlitEffect()
{
	D3D12_GRAPHICS_PIPELINE_STATE_DESC desc = PipelineState::getDefaultFullScreenState();
	desc.PS = Shader::fullScreenPS->getByteCode();
	desc.NumRenderTargets = 1;
	desc.RTVFormats[0] = Graphics::backBufferFormat;

	GraphicsDevice::get()->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(&backBufferBlitState));

	LOGSUCCESS("create", Logger::brightMagenta, "BackBufferBlitEffect", Logger::resetColor(), "succeeded");
}

BackBufferBlitEffect::~BackBufferBlitEffect()
{
}
