#include<Gear/Core/Graphics.h>

UINT Graphics::frameBufferCount = 0;

UINT Graphics::frameIndex = 0;

UINT Graphics::width = 0;

UINT Graphics::height = 0;

float Graphics::aspectRatio = 0.f;

float Graphics::deltaTime = 0.f;

float Graphics::timeElapsed = 0.f;

UINT Graphics::getFrameBufferCount()
{
	return frameBufferCount;
}

UINT Graphics::getFrameIndex()
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

UINT Graphics::getWidth()
{
	return width;
}

UINT Graphics::getHeight()
{
	return height;
}

FLOAT Graphics::getAspectRatio()
{
	return aspectRatio;
}
