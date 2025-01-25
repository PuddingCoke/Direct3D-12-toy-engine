#pragma once

#ifndef _GRAPHICS_H_
#define _GRAPHICS_H_

#include<Gear/Core/GraphicsDevice.h>

class Graphics
{
public:

	static uint32_t getFrameBufferCount();

	static uint32_t getFrameIndex();

	static float getDeltaTime();

	static float getTimeElapsed();

	static uint32_t getWidth();

	static uint32_t getHeight();

	static float getAspectRatio();

	static constexpr DXGI_FORMAT backBufferFormat = DXGI_FORMAT_B8G8R8A8_UNORM;

private:

	friend class RenderEngine;

	static uint32_t frameBufferCount;

	static uint32_t frameIndex;

	static uint32_t width;

	static uint32_t height;

	static float aspectRatio;

	static float deltaTime;

	static float timeElapsed;

};

#endif // !_GRAPHICS_H_