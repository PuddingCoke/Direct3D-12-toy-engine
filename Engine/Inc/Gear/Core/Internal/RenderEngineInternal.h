#pragma once

#ifndef _GEAR_CORE_RENDERENGINE_INTERNAL_H_
#define _GEAR_CORE_RENDERENGINE_INTERNAL_H_

#include<Gear/Core/D3D12Resource/ReadbackHeap.h>

namespace Gear::Core::RenderEngine::Internal
{
	void initialize(const uint32_t width, const uint32_t height, const HWND hwnd, const bool useSwapChainBuffer, const bool initializeImGuiSurface);

	void release();

	struct InitializeToken
	{
		InitializeToken(const uint32_t width, const uint32_t height, const HWND hwnd, const bool useSwapChainBuffer, const bool initializeImGuiSurface)
		{
			initialize(width, height, hwnd, useSwapChainBuffer, initializeImGuiSurface);
		}

		~InitializeToken()
		{
			release();
		}
	};

	void updateFrameIndex();

	void waitDestroyable();

	void waitFrameGPUComplete();

	void waitFrameCPUReusable();

	//开始一帧
	//后备缓冲被转换到STATE_RENDER_TARGET
	void beginFrame();

	//结束这一帧，调用ExecuteCommandLists
	//后备缓冲被转换到STATE_PRESENT
	void endFrame();

	void present();

	void setDeltaTime(const float deltaTime);

	//更新总时间
	void updateTimeElapsed();

	//使总帧数渲染自增1
	void renderedFrameCountInc();

	void saveBackBuffer(D3D12Resource::ReadbackHeap* const readbackHeap);

	void setDefRenderTexture();

	void setRenderTexture(D3D12Resource::Texture* const renderTexture, const D3D12_CPU_DESCRIPTOR_HANDLE handle);

	void initializeResources();
}

#endif // !_GEAR_CORE_RENDERENGINE_INTERNAL_H_
