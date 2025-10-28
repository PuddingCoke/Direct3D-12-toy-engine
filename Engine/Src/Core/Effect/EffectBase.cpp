#include<Gear/Core/Effect/EffectBase.h>

Core::Effect::EffectBase::EffectBase(GraphicsContext* const context, const uint32_t width, const uint32_t height, const DXGI_FORMAT format) :
	outputTexture(ResourceManager::createTextureRenderView(width, height, format, 1, 1, false, true, format, DXGI_FORMAT_UNKNOWN, format)),
	context(context),
	width(width),
	height(height)
{
}

Core::Effect::EffectBase::~EffectBase()
{
	delete outputTexture;
}
