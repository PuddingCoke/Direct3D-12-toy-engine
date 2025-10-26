#pragma once

#ifndef _CORE_RENDERENGINE_INTERNAL_H_
#define _CORE_RENDERENGINE_INTERNAL_H_

namespace Core
{
	namespace RenderEngine
	{
		namespace Internal
		{

			void initialize(const uint32_t width, const uint32_t height, const HWND hwnd, const bool useSwapChainBuffer, const bool initializeImGuiSurface);

			void release();

			void waitForCurrentFrame();

			void waitForNextFrame();

			void begin();

			void end();

			void present();

			void setDeltaTime(const float deltaTime);

			void updateTimeElapsed();

			void saveBackBuffer(ReadbackHeap* const readbackHeap);

			void setDefRenderTexture();

			void setRenderTexture(Texture* const renderTexture, const D3D12_CPU_DESCRIPTOR_HANDLE handle);

			void initializeResources();

		}
	}
}

#endif // !_CORE_RENDERENGINE_INTERNAL_H_
