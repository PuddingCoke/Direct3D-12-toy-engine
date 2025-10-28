#pragma once

#ifndef _CORE_GRAPHICS_INTERNAL_H_
#define _CORE_GRAPHICS_INTERNAL_H_

#include<cstdint>

namespace Core
{
	namespace Graphics
	{
		namespace Internal
		{

			void initialize(const uint32_t frameBufferCount, const uint32_t width, const uint32_t height);

			void renderedFrameCountInc();

			void setFrameIndex(const uint32_t frameIndex);

			void setDeltaTime(const float deltaTime);

			void updateTimeElapsed();

			void setBackBufferHandle(const D3D12_CPU_DESCRIPTOR_HANDLE backBufferHandle);

			void setReservedGlobalCBuffer(Resource::ImmutableCBuffer* const reservedGlobalCBuffer);

		}
	}
}

#endif // !_CORE_GRAPHICS_INTERNAL_H_
