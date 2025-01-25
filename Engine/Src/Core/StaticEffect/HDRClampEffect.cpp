#include<Gear/Core/StaticEffect/HDRClampEffect.h>

#include<Gear/CompiledShaders/HDRClampCS.h>

HDRClampEffect* HDRClampEffect::instance = nullptr;

HDRClampEffect* HDRClampEffect::get()
{
	return instance;
}

void HDRClampEffect::process(GraphicsContext* const context, TextureRenderView* const inOutTexture)
{
	if (inOutTexture->getTexture()->getFormat() == DXGI_FORMAT_R16G16B16A16_FLOAT)
	{
		context->setPipelineState(hdrClampState.Get());

		context->setCSConstants({ inOutTexture->getUAVMipIndex(0) }, 0);

		context->transitionResources();

		context->dispatch(inOutTexture->getTexture()->getWidth() / 16 + 1, inOutTexture->getTexture()->getHeight() / 16 + 1, 1);

		context->uavBarrier({ inOutTexture->getTexture() });
	}
}

HDRClampEffect::HDRClampEffect()
{
	hdrClampShader = new Shader(g_HDRClampCSBytes, sizeof(g_HDRClampCSBytes));

	PipelineState::createComputeState(&hdrClampState, hdrClampShader);

	LOGSUCCESS("create", Logger::brightMagenta, "HDRClampEffect", Logger::resetColor(), "succeeded");
}

HDRClampEffect::~HDRClampEffect()
{
	if (hdrClampShader)
	{
		delete hdrClampShader;
	}
}
