#pragma once

#ifndef _GRAPHICS_H_
#define _GRAPHICS_H_

#include<Gear/Utils/FMT.h>

namespace Graphics
{

	uint32_t getFrameBufferCount();

	uint32_t getFrameIndex();

	float getDeltaTime();

	float getTimeElapsed();

	uint32_t getWidth();

	uint32_t getHeight();

	float getAspectRatio();

	uint64_t getRenderedFrameCount();

	constexpr DXGI_FORMAT backBufferFormat = FMT::BGRA8UN;

}

#endif // !_GRAPHICS_H_