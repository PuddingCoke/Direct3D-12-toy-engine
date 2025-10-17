#pragma once

#ifndef _GRAPHICS_H_
#define _GRAPHICS_H_

#include<Gear/Utils/FMT.h>

class Graphics
{
public:

	Graphics() = delete;

	Graphics(const Graphics&) = delete;

	void operator=(const Graphics&) = delete;

	static uint32_t getFrameBufferCount();

	static uint32_t getFrameIndex();

	static float getDeltaTime();

	static float getTimeElapsed();

	static uint32_t getWidth();

	static uint32_t getHeight();

	static float getAspectRatio();

	static uint64_t getRenderedFrameCount();

	static constexpr DXGI_FORMAT backBufferFormat = FMT::BGRA8UN;

private:

	friend class RenderEngine;

	static void initialize(const uint32_t frameBufferCount, const uint32_t width, const uint32_t height);

	static void renderedFrameCountInc();

	static uint32_t frameBufferCount;

	static uint32_t frameIndex;

	static uint32_t width;

	static uint32_t height;

	static uint64_t renderedFrameCount;

	static float aspectRatio;

	static float deltaTime;

	static float timeElapsed;

};

#endif // !_GRAPHICS_H_