#include<Gear/Core/Graphics.h>

#include<Gear/Core/Internal/GraphicsInternal.h>

namespace
{
	struct GraphicsPrivate
	{

		uint32_t frameBufferCount = 0;

		uint32_t frameIndex = 0;

		uint32_t width = 0;

		uint32_t height = 0;

		uint64_t renderedFrameCount = 0;

		float aspectRatio = 0.f;

		float deltaTime = 0.f;

		float timeElapsed = 0.f;

	}pvt;
}

uint32_t Graphics::getFrameBufferCount()
{
	return pvt.frameBufferCount;
}

uint32_t Graphics::getFrameIndex()
{
	return pvt.frameIndex;
}

float Graphics::getDeltaTime()
{
	return pvt.deltaTime;
}

float Graphics::getTimeElapsed()
{
	return pvt.timeElapsed;
}

uint32_t Graphics::getWidth()
{
	return pvt.width;
}

uint32_t Graphics::getHeight()
{
	return pvt.height;
}

float Graphics::getAspectRatio()
{
	return pvt.aspectRatio;
}

uint64_t Graphics::getRenderedFrameCount()
{
	return pvt.renderedFrameCount;
}

void Graphics::Internal::initialize(const uint32_t frameBufferCount, const uint32_t width, const uint32_t height)
{
	pvt.frameBufferCount = frameBufferCount;

	pvt.width = width;

	pvt.height = height;

	pvt.aspectRatio = static_cast<float>(width) / static_cast<float>(height);
}

void Graphics::Internal::renderedFrameCountInc()
{
	pvt.renderedFrameCount++;
}

void Graphics::Internal::setFrameIndex(const uint32_t frameIndex)
{
	pvt.frameIndex = frameIndex;
}

void Graphics::Internal::setDeltaTime(const float deltaTime)
{
	pvt.deltaTime = deltaTime;
}

void Graphics::Internal::updateTimeElapsed()
{
	pvt.timeElapsed += pvt.deltaTime;
}