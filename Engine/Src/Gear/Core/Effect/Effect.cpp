#include<Gear/Core/Effect/Effect.h>

Effect::Effect(GraphicsContext* const context, const UINT width, const UINT height, const DXGI_FORMAT format) :
	outputTexture(new TextureRenderTarget(width, height, format, 1, 1, false, true, format, DXGI_FORMAT_UNKNOWN, format)),
	context(context),
	width(width),
	height(height)
{
}

Effect::~Effect()
{
	delete outputTexture;
}
