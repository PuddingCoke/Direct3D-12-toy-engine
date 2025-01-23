#include<Gear/Core/StaticEffect/HDRClampEffect.h>

HDRClampEffect* HDRClampEffect::instance = nullptr;

HDRClampEffect* HDRClampEffect::get()
{
	return instance;
}

void HDRClampEffect::process(GraphicsContext* const context, TextureRenderView* const inOutTexture)
{
	context->setPipelineState(hdrClampState.Get());

	context->setCSConstants({ inOutTexture->getUAVMipIndex(0) }, 0);

	context->transitionResources();

	context->dispatch(inOutTexture->getTexture()->getWidth() / 16 + 1, inOutTexture->getTexture()->getHeight() / 16 + 1, 1);

	context->uavBarrier({ inOutTexture->getTexture() });
}

void HDRClampEffect::initialize()
{
	instance = new HDRClampEffect();
}

void HDRClampEffect::release()
{
	if (instance)
	{
		delete instance;
	}
}

HDRClampEffect::HDRClampEffect()
{
	hdrClampShader = new Shader(g_HDRClampCSBytes, sizeof(g_HDRClampCSBytes));

	PipelineState::createComputeState(&hdrClampState, hdrClampShader);
}

HDRClampEffect::~HDRClampEffect()
{
	if (hdrClampShader)
	{
		delete hdrClampShader;
	}
}
