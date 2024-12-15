#pragma once

#ifndef _RENDERENGINE_H_
#define _RENDERENGINE_H_

#include<ImGUI/imgui.h>
#include<ImGUI/imgui_impl_win32.h>
#include<ImGUI/imgui_impl_dx12.h>

#include<Gear/Core/ConstantBufferManager.h>
#include<Gear/Core/GraphicsContext.h>
#include<Gear/Core/ResourceManager.h>
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

	void submitCommandList(CommandList* const commandList);

	GPUVendor getVendor() const;

	Texture* getCurrentRenderTexture() const;

	bool getDisplayImGuiSurface() const;

private:

	friend class Gear;

	static RenderEngine* instance;

	ComPtr<IDXGIAdapter4> getBestAdapterAndVendor(IDXGIFactory7* const factory);

	void processCommandLists();

	void waitForPreviousFrame();

	void waitForNextFrame();

	void begin();

	void end();

	void updateConstantBuffer();

	void initializeResources();

	void toggleImGuiSurface();

	void beginImGuiFrame() const;

	void drawImGuiFrame(CommandList* const targetCommandList);

	RenderEngine(const HWND hwnd, const bool useSwapChainBuffer, const bool initializeImGuiSurface);

	~RenderEngine();

	const bool initializeImGuiSurface;

	bool displayImGUISurface;

	GPUVendor vendor;

	ComPtr<IDXGISwapChain4> swapChain;

	ComPtr<ID3D12CommandQueue> commandQueue;

	std::vector<CommandList*> recordCommandLists;

	ComPtr<ID3D12Fence> fence;

	UINT64 fenceValues[Graphics::FrameBufferCount];

	HANDLE fenceEvent;

	CommandList* prepareCommandList;

	Texture* backBufferResources[Graphics::FrameBufferCount];

	std::mutex submitCommandListLock;

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