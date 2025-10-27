#include<Gear/Core/StaticEffect/HDRClampEffect.h>

#include<Gear/Core/StaticEffect/Internal/HDRClampEffectInternal.h>

#include<Gear/CompiledShaders/HDRClampCS.h>

namespace
{
	struct HDRClampEffectPrivate
	{

		Core::Shader* hdrClampShader;

		ComPtr<ID3D12PipelineState> hdrClampState;

	}pvt;
}

void Core::StaticEffect::HDRClampEffect::process(GraphicsContext* const context, TextureRenderView* const inOutTexture)
{
	if (inOutTexture->getTexture()->getFormat() == DXGI_FORMAT_R16G16B16A16_FLOAT)
	{
		context->setPipelineState(pvt.hdrClampState.Get());

		context->setCSConstants({ inOutTexture->getUAVMipIndex(0) }, 0);

		context->transitionResources();

		context->dispatch(inOutTexture->getTexture()->getWidth() / 16 + 1, inOutTexture->getTexture()->getHeight() / 16 + 1, 1);

		context->uavBarrier({ inOutTexture->getTexture() });
	}
}

void Core::StaticEffect::HDRClampEffect::Internal::initialize()
{
	pvt.hdrClampShader = new Core::Shader(g_HDRClampCSBytes, sizeof(g_HDRClampCSBytes));

	Core::PipelineStateHelper::createComputeState(&pvt.hdrClampState, pvt.hdrClampShader);

	LOGSUCCESS(L"create", LogColor::brightMagenta, L"HDRClampEffect", LogColor::defaultColor, L"succeeded");
}

void Core::StaticEffect::HDRClampEffect::Internal::release()
{
	if (pvt.hdrClampShader)
	{
		delete pvt.hdrClampShader;
	}

	pvt.hdrClampState = nullptr;
}
