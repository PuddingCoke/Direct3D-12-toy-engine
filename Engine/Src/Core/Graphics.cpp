#include<Gear/Core/Graphics.h>

#include<Gear/Core/GraphicsInternal.h>

namespace
{

	uint32_t g_frameBufferCount = 0;

	uint32_t g_frameIndex = 0;

	uint32_t g_width = 0;

	uint32_t g_height = 0;

	uint64_t g_renderedFrameCount = 0;

	float g_aspectRatio = 0.f;

	float g_deltaTime = 0.f;

	float g_timeElapsed = 0.f;

}

uint32_t Graphics::getFrameBufferCount()
{
	return g_frameBufferCount;
}

uint32_t Graphics::getFrameIndex()
{
	return g_frameIndex;
}

float Graphics::getDeltaTime()
{
	return g_deltaTime;
}

float Graphics::getTimeElapsed()
{
	return g_timeElapsed;
}

uint32_t Graphics::getWidth()
{
	return g_width;
}

uint32_t Graphics::getHeight()
{
	return g_height;
}

float Graphics::getAspectRatio()
{
	return g_aspectRatio;
}

uint64_t Graphics::getRenderedFrameCount()
{
	return g_renderedFrameCount;
}

void Graphics::Internal::initialize(const uint32_t frameBufferCount, const uint32_t width, const uint32_t height)
{
	g_frameBufferCount = frameBufferCount;

	g_width = width;

	g_height = height;

	g_aspectRatio = static_cast<float>(width) / static_cast<float>(height);
}

void Graphics::Internal::renderedFrameCountInc()
{
	g_renderedFrameCount++;
}

void Graphics::Internal::setFrameIndex(const uint32_t frameIndex)
{
	g_frameIndex = frameIndex;
}

void Graphics::Internal::setDeltaTime(const float deltaTime)
{
	g_deltaTime = deltaTime;
}

void Graphics::Internal::updateTimeElapsed()
{
	g_timeElapsed += g_deltaTime;
}