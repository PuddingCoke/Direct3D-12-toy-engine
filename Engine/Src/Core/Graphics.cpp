#include<Gear/Core/Graphics.h>

uint32_t Graphics::frameBufferCount = 0;

uint32_t Graphics::frameIndex = 0;

uint32_t Graphics::width = 0;

uint32_t Graphics::height = 0;

uint64_t Graphics::renderedFrameCount = 0;

float Graphics::aspectRatio = 0.f;

float Graphics::deltaTime = 0.f;

float Graphics::timeElapsed = 0.f;

uint32_t Graphics::getFrameBufferCount()
{
	return frameBufferCount;
}

uint32_t Graphics::getFrameIndex()
{
	return frameIndex;
}

float Graphics::getDeltaTime()
{
	return deltaTime;
}

float Graphics::getTimeElapsed()
{
	return timeElapsed;
}

uint32_t Graphics::getWidth()
{
	return width;
}

uint32_t Graphics::getHeight()
{
	return height;
}

float Graphics::getAspectRatio()
{
	return aspectRatio;
}

uint64_t Graphics::getRenderedFrameCount()
{
	return renderedFrameCount;
}

void Graphics::initialize(const uint32_t frameBufferCount, const uint32_t width, const uint32_t height)
{
	Graphics::frameBufferCount = frameBufferCount;

	Graphics::width = width;

	Graphics::height = height;

	Graphics::aspectRatio = static_cast<float>(width) / static_cast<float>(height);
}

void Graphics::renderedFrameCountInc()
{
	Graphics::renderedFrameCount++;
}
