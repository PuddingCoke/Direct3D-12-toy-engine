#pragma once

#ifndef _GRAPHICSINTERNAL_H_
#define _GRAPHICSINTERNAL_H_

#include<cstdint>

namespace Graphics
{

	namespace Internal
	{

		void initialize(const uint32_t frameBufferCount, const uint32_t width, const uint32_t height);

		void renderedFrameCountInc();

		void setFrameIndex(const uint32_t frameIndex);

		void setDeltaTime(const float deltaTime);

		void updateTimeElapsed();

	}

}

#endif // !_GRAPHICSINTERNAL_H_
