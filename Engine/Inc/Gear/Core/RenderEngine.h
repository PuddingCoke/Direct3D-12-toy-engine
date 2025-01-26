#pragma once

#ifndef _RENDERENGINE_H_
#define _RENDERENGINE_H_

#include<Gear/Core/CommandList.h>

#include<Gear/Core/Camera.h>

#include<ImGUI/imgui.h>

#include<ImGUI/imgui_impl_win32.h>

#include<ImGUI/imgui_impl_dx12.h>

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

	RenderEngine() = delete;

	RenderEngine(const RenderEngine&) = delete;

	void operator=(const RenderEngine&) = delete;

	static RenderEngine* get();

	void submitCommandList(CommandList* const commandList);

	GPUVendor getVendor() const;

	Texture* getRenderTexture() const;

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

	void present() const;

	void updateConstantBuffer() const;

	void initializeResources();

	void toggleImGuiSurface();

	void beginImGuiFrame() const;

	void drawImGuiFrame(CommandList* const targetCommandList);

	void setDefRenderTexture();

	void setRenderTexture(Texture* const renderTexture, const D3D12_CPU_DESCRIPTOR_HANDLE handle);

	void setDeltaTime(const float deltaTime) const;

	void updateTimeElapsed() const;

	RenderEngine(const uint32_t width, const uint32_t height, const HWND hwnd, const bool useSwapChainBuffer, const bool initializeImGuiSurface);

	~RenderEngine();

	const bool initializeImGuiSurface;

	bool displayImGUISurface;

	GPUVendor vendor;

	ComPtr<IDXGISwapChain4> swapChain;

	ComPtr<ID3D12CommandQueue> commandQueue;

	std::vector<CommandList*> recordCommandLists;

	ComPtr<ID3D12Fence> fence;

	uint64_t* fenceValues;

	HANDLE fenceEvent;

	CommandList* prepareCommandList;

	Texture** backBufferTextures;

	D3D12_CPU_DESCRIPTOR_HANDLE* backBufferHandles;

	Texture* renderTexture;

	std::mutex submitCommandListLock;

	int32_t syncInterval;

	struct PerFrameResource
	{
		float deltaTime;
		float timeElapsed;
		uint32_t uintSeed;
		float floatSeed;
		Camera::CameraMatrices matrices;
		DirectX::XMFLOAT2 screenSize;
		DirectX::XMFLOAT2 screenTexelSize;
		DirectX::XMFLOAT4 padding[41];
	} perFrameResource;

};

#endif // !_RENDERENGINE_H_