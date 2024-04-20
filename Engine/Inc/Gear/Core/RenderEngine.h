#pragma once

#ifndef _RENDERENGINE_H_
#define _RENDERENGINE_H_

#include<Gear/Core/RenderPass.h>
#include<Gear/Core/Graphics.h>
#include<Gear/Core/Camera.h>
#include<Gear/Core/Shader.h>
#include<Gear/Core/States.h>
#include<Gear/Core/PipelineState.h>

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

	//no present
	void waitForPreviousFrame();

	//has present
	void waitForNextFrame();

	void begin();

	void end();

	//push beginCommandlist in advance
	void updateConstantBuffer();

	GPUVendor getVendor() const;

	Texture* getCurrentRenderTexture() const;

private:

	friend class Gear;

	static RenderEngine* instance;

	ComPtr<IDXGIAdapter4> getBestAdapterAndVendor(IDXGIFactory7* const factory);

	RenderEngine(const HWND hwnd, const bool useSwapChainBuffer);

	~RenderEngine();

	GPUVendor vendor;

	ComPtr<IDXGISwapChain4> swapChain;

	ComPtr<ID3D12CommandQueue> commandQueue;

	std::vector<ID3D12CommandList*> commandLists;

	ComPtr<ID3D12Fence> fence;

	UINT64 fenceValues[Graphics::FrameBufferCount];

	HANDLE fenceEvent;

	CommandList* beginCommandlist;

	CommandList* endCommandList;

	Texture* backBufferResources[Graphics::FrameBufferCount];

	struct PerFrameResource
	{
		Graphics::Time time;
		Camera::CameraMatrices matrices;
		DirectX::XMFLOAT2 screenSize;
		DirectX::XMFLOAT2 screenTexelSize;
		DirectX::XMFLOAT4 padding[41];
	} perFrameResource;

};

#endif // !_RENDERENGINE_H_