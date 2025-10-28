#include<Gear/Core/Resource/SwapTexture.h>

Core::Resource::SwapTexture::SwapTexture(const std::function<TextureRenderView* (void)>& factoryFunc) :
	texture0(factoryFunc()), texture1(factoryFunc()),
	width(texture0->getTexture()->getWidth()),
	height(texture0->getTexture()->getHeight())
{
}

Core::Resource::SwapTexture::~SwapTexture()
{
	delete texture0;
	delete texture1;
}

Core::Resource::TextureRenderView* Core::Resource::SwapTexture::read() const
{
	return texture0;
}

Core::Resource::TextureRenderView* Core::Resource::SwapTexture::write() const
{
	return texture1;
}

void Core::Resource::SwapTexture::swap()
{
	TextureRenderView* const temp = texture0;
	texture0 = texture1;
	texture1 = temp;
}
