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

	void submitRecordCommandList(CommandList* const commandList);

	void submitCreateCommandList(CommandList* const commandList);

	GPUVendor getVendor() const;

	Texture* getCurrentRenderTexture() const;

	bool getDisplayImGuiSurface() const;

private:

	friend class Gear;

	static RenderEngine* instance;

	ComPtr<IDXGIAdapter4> getBestAdapterAndVendor(IDXGIFactory7* const factory);

	//merge recordCommandLists and createCommandLists
	//and execute all commandLists by using one ExecuteCommandLists call
	void processCommandLists();

	//just wait for previous frame complete and this does not present frame buffer
	//becuase this method is created for spceial purpose such as wait for resource creation
	void waitForPreviousFrame();

	//wait for next frame complete and present frame buffer
	void waitForNextFrame();

	//we use this method to make sure beginCommandList is always the 1th element in recordCommandLists
	//and this is also the start of imGui frame
	void begin();

	//using beginCommandList transition back buffer to STATE_RENDER_TARGET
	//update all constant buffers
	//using endCommandList transition back buffer to STATE_PRESENT
	//draw imGui frame
	//finally execute all commandLists
	void end();

	//make sure begin command list is the 1th element in recordCommandLists
	//this method will update all constant buffers by using copybufferregion
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

	//after the completion of render pass's recordCommand method
	//render pass's commandList will be pushed to this container
	std::vector<CommandList*> recordCommandLists;

	//it is potential to create render pass on the fly
	//i want to make this asynchronously
	//main render thread will loop through this container and solve pending barriers one by one
	std::vector<CommandList*> createCommandLists;

	//we need a mutex protect createCommandLists container
	std::mutex createCommandListsMutex;

	ComPtr<ID3D12Fence> fence;

	UINT64 fenceValues[Graphics::FrameBufferCount];

	HANDLE fenceEvent;

	//do some preperation works and solve pending barriers
	CommandList* beginCommandList;

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