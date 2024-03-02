#pragma once

#ifndef _RENDERENGINE_H_
#define _RENDERENGINE_H_

#include<Gear/Core/RenderPass.h>
#include<Gear/Core/Graphics.h>
#include<Gear/Core/Camera.h>

#include<iostream>
#include<dxgi1_6.h>

enum class GPUVendor
{
	NVIDIA,
	AMD,
	INTEL,
	UNKNOWN
};

class RenderEngine
{
public:

	static RenderEngine* get();

	void submitRenderPass(RenderPass* const pass);

	void processCommandLists();

	void waitForGPU();

	void begin();

	void end();

	GPUVendor getVendor() const;

private:

	friend class Gear;

	static RenderEngine* instance;

	RenderEngine(const HWND hwnd);

	~RenderEngine();

	GPUVendor vendor;

	ComPtr<IDXGISwapChain4> swapChain;

	ComPtr<ID3D12CommandQueue> commandQueue;

	std::vector<ID3D12CommandList*> commandLists;

	ComPtr<ID3D12Fence> fence;

	UINT64 fenceValues[Graphics::FrameBufferCount];

	HANDLE fenceEvent;

	//two commandlists handle global resources transition
	//beginCommandList handle resources creation

	CommandList* beginCommandlist;

	CommandList* endCommandList;

	ComPtr<ID3D12Resource> backBufferResources[Graphics::FrameBufferCount];

	std::vector<Resource*> transientResources[Graphics::FrameBufferCount];

	struct PerFrameResource
	{
		Graphics::Time time;
		Camera::CameraMatrices matrices;
	} perFrameResource;

};

#endif // !_RENDERENGINE_H_