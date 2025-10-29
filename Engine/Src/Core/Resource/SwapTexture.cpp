#include<Gear/Core/Resource/SwapTexture.h>

Gear::Core::Resource::SwapTexture::SwapTexture(const std::function<TextureRenderView* (void)>& factoryFunc) :
	texture0(factoryFunc()), texture1(factoryFunc()),
	width(texture0->getTexture()->getWidth()),
	height(texture0->getTexture()->getHeight())
{
}

Gear::Core::Resource::SwapTexture::~SwapTexture()
{
	delete texture0;
	delete texture1;
}

Gear::Core::Resource::TextureRenderView* Gear::Core::Resource::SwapTexture::read() const
{
	return texture0;
}

Gear::Core::Resource::TextureRenderView* Gear::Core::Resource::SwapTexture::write() const
{
	return texture1;
}

void Gear::Core::Resource::SwapTexture::swap()
{
	TextureRenderView* const temp = texture0;
	texture0 = texture1;
	texture1 = temp;
}
