﻿#pragma once

#ifndef _CORE_GRAPHICS_H_
#define _CORE_GRAPHICS_H_

#include<Gear/Core/FMT.h>

#include<Gear/Core/Resource/ImmutableCBuffer.h>

namespace Core
{
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

		D3D12_CPU_DESCRIPTOR_HANDLE getBackBufferHandle();

		Resource::ImmutableCBuffer* getReservedGlobalCBuffer();

		constexpr DXGI_FORMAT backBufferFormat = FMT::BGRA8UN;

	}
}

#endif // !_CORE_GRAPHICS_H_