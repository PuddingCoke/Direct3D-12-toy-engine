#include<Gear/Core/Resource/SwapTexture.h>

SwapTexture::SwapTexture(const std::function<TextureRenderTarget* (void)>& factoryFunc) :
	texture0(factoryFunc()), texture1(factoryFunc()),
	width(texture0->getTexture()->getWidth()),
	height(texture0->getTexture()->getHeight())
{
}

SwapTexture::~SwapTexture()
{
	delete texture0;
	delete texture1;
}

TextureRenderTarget* SwapTexture::read() const
{
	return texture0;
}

TextureRenderTarget* SwapTexture::write() const
{
	return texture1;
}

void SwapTexture::swap()
{
	TextureRenderTarget* const temp = texture0;
	texture0 = texture1;
	texture1 = temp;
}
