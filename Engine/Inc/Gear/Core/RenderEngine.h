#pragma once

#ifndef _RENDERENGINE_H_
#define _RENDERENGINE_H_

#include<Gear/Core/RenderPass.h>

#include<dxgi1_6.h>

class RenderEngine
{
public:

	static RenderEngine* get();

	void submitRenderPass(RenderPass* const pass);

	void processCommandLists();

	void present();

private:

	friend class Gear;

	static RenderEngine* instance;

	ComPtr<IDXGISwapChain4> swapChain;

	ComPtr<ID3D12CommandQueue> commandQueue;

	std::vector<ID3D12CommandList*> commandLists;

	ComPtr<ID3D12Fence> fence;

	UINT64 fenceValues[Graphics::FrameBufferCount];

	HANDLE fenceEvent;
};

#endif // !_RENDERENGINE_H_