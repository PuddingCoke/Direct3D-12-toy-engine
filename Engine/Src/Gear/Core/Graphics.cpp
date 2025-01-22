#include<Gear/Core/Graphics.h>

uint32_t Graphics::frameBufferCount = 0;

uint32_t Graphics::frameIndex = 0;

uint32_t Graphics::width = 0;

uint32_t Graphics::height = 0;

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
