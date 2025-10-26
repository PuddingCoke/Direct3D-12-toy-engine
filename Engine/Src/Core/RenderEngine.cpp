#include<Gear/Core/RenderEngine.h>

#include<Gear/Core/Internal/RenderEngineInternal.h>

#include<Gear/Core/DynamicCBufferManager.h>

#include<Gear/Core/GlobalRootSignature.h>

#include<Gear/Core/Graphics.h>

#include<Gear/Utils/Random.h>

#include<Gear/Core/Internal/GraphicsInternal.h>

#include<Gear/Core/Internal/GraphicsDeviceInternal.h>

#include<Gear/Core/Internal/GlobalDescriptorHeapInternal.h>

#include<Gear/Core/Internal/GlobalRootSignatureInternal.h>

#include<Gear/Core/Internal/GlobalShaderInternal.h>

#include<Gear/Core/Internal/MainCameraInternal.h>

#include<Gear/Core/Internal/DynamicCBufferManagerInternal.h>

#include<Gear/Core/StaticEffect/Internal/BackBufferBlitEffectInternal.h>

#include<Gear/Core/StaticEffect/Internal/HDRClampEffectInternal.h>

#include<Gear/Core/StaticEffect/Internal/LatLongMapToCubeMapEffectInternal.h>

namespace
{
	class RenderEnginePrivate
	{
	public:

		RenderEnginePrivate() = delete;

		RenderEnginePrivate(const RenderEnginePrivate&) = delete;

		void operator=(const RenderEnginePrivate&) = delete;

		RenderEnginePrivate(const uint32_t width, const uint32_t height, const HWND hwnd, const bool useSwapChainBuffer, const bool initializeImGuiSurface);

		~RenderEnginePrivate();

		void submitCommandList(CommandList* const commandList);

		Core::GPUVendor getVendor() const;

		Texture* getRenderTexture() const;

		bool getDisplayImGuiSurface() const;

		void waitForCurrentFrame();

		void waitForNextFrame();

		void begin();

		void end();

		void present() const;

		void setDeltaTime(const float deltaTime) const;

		void updateTimeElapsed() const;

		void setDefRenderTexture();

		void setRenderTexture(Texture* const renderTexture, const D3D12_CPU_DESCRIPTOR_HANDLE handle);

		void initializeResources();

		void saveBackBuffer(ReadbackHeap* const readbackHeap);

	private:

		ComPtr<IDXGIAdapter4> getBestAdapterAndVendor(IDXGIFactory7* const factory);

		void processCommandLists();

		void updateConstantBuffer() const;

		void toggleImGuiSurface();

		void beginImGuiFrame() const;

		void drawImGuiFrame(CommandList* const targetCommandList);

		void createStaticResources();

		void releaseStaticResources();

		const bool initializeImGuiSurface;

		bool displayImGUISurface;

		Core::GPUVendor vendor;

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

		StaticCBuffer* reservedGlobalCBuffer;

		ResourceManager* resManager;

		struct PerframeResource
		{
			float deltaTime;
			float timeElapsed;
			uint32_t uintSeed;
			float floatSeed;
			Core::MainCamera::Internal::Matrices matrices;
			DirectX::XMFLOAT2 screenSize;
			DirectX::XMFLOAT2 screenTexelSize;
			DirectX::XMFLOAT4 padding[9];
		}perframeResource;

	}*pvt = nullptr;
}

RenderEnginePrivate::RenderEnginePrivate(const uint32_t width, const uint32_t height, const HWND hwnd, const bool useSwapChainBuffer, const bool initializeImGuiSurface) :
	backBufferTextures(nullptr),
	backBufferHandles(nullptr),
	fenceEvent(CreateEvent(nullptr, FALSE, FALSE, nullptr)),
	vendor(Core::GPUVendor::UNKNOWN),
	initializeImGuiSurface(initializeImGuiSurface),
	displayImGUISurface(false),
	syncInterval(1),
	resManager(nullptr),
	perframeResource{}
{
	Core::Graphics::Internal::initialize(useSwapChainBuffer ? 3 : 1, width, height);

	ComPtr<IDXGIFactory7> factory;

#ifdef _DEBUG
	LOGENGINE(LogColor::brightGreen, L"enable", LogColor::defaultColor, L"debug layer");

	ComPtr<ID3D12Debug> debugController;

	D3D12GetDebugInterface(IID_PPV_ARGS(&debugController));

	debugController->EnableDebugLayer();

	CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, IID_PPV_ARGS(&factory));
#else
	LOGENGINE(LogColor::brightRed, L"disable", LogColor::defaultColor, L"debug layer");

	CreateDXGIFactory2(0, IID_PPV_ARGS(&factory));
#endif // _DEBUG

	ComPtr<IDXGIAdapter4> adapter = getBestAdapterAndVendor(factory.Get());

	Core::GraphicsDevice::Internal::initialize(adapter.Get());

	Core::GraphicsDevice::Internal::checkFeatureSupport();

	{
		D3D12_COMMAND_QUEUE_DESC queueDesc = {};
		queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_DISABLE_GPU_TIMEOUT;

		Core::GraphicsDevice::get()->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&commandQueue));

		commandQueue->SetName(L"Graphics Command Queue");
	}

	Core::GlobalShader::Internal::initialize();

	Core::GlobalDescriptorHeap::Internal::initialize();

	Core::GlobalRootSignature::Internal::initialize();

	Core::DynamicCBufferManager::Internal::initialize();

	{
		DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
		swapChainDesc.BufferCount = useSwapChainBuffer ? Core::Graphics::getFrameBufferCount() : 2;
		swapChainDesc.Width = Core::Graphics::getWidth();
		swapChainDesc.Height = Core::Graphics::getHeight();
		swapChainDesc.Format = Core::Graphics::backBufferFormat;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		swapChainDesc.SampleDesc.Count = 1;
		swapChainDesc.SampleDesc.Quality = 0;

		ComPtr<IDXGISwapChain1> swapChain1;

		factory->CreateSwapChainForHwnd(commandQueue.Get(), hwnd, &swapChainDesc, nullptr, nullptr, &swapChain1);

		factory->MakeWindowAssociation(hwnd, DXGI_MWA_NO_ALT_ENTER);

		swapChain1.As(&swapChain);
	}

	fenceValues = new uint64_t[Core::Graphics::getFrameBufferCount()];

	for (uint32_t i = 0; i < Core::Graphics::getFrameBufferCount(); i++)
	{
		fenceValues[i] = 0;
	}

	Core::GraphicsDevice::get()->CreateFence(fenceValues[Core::Graphics::getFrameIndex()], D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));

	fenceValues[Core::Graphics::getFrameIndex()]++;

	prepareCommandList = new CommandList(D3D12_COMMAND_LIST_TYPE_DIRECT);

	reservedGlobalCBuffer = ResourceManager::createStaticCBuffer(sizeof(PerframeResource), true);

	reservedGlobalCBuffer->getBuffer()->setName(L"Reserved Global CBuffer");

	Core::Graphics::Internal::setReservedGlobalCBuffer(reservedGlobalCBuffer);

	//确保准备命令列表总是处于容器第一个位置
	begin();

	{
		prepareCommandList->pushResourceTrackList(reservedGlobalCBuffer->getBuffer());

		reservedGlobalCBuffer->getBuffer()->setState(D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);

		prepareCommandList->transitionResources();
	}

	if (useSwapChainBuffer)
	{
		DescriptorHandle descriptorHandle = Core::GlobalDescriptorHeap::getRenderTargetHeap()->allocStaticDescriptor(Core::Graphics::getFrameBufferCount());

		backBufferTextures = new Texture * [Core::Graphics::getFrameBufferCount()];

		backBufferHandles = new D3D12_CPU_DESCRIPTOR_HANDLE[Core::Graphics::getFrameBufferCount()];

		for (uint32_t i = 0; i < Core::Graphics::getFrameBufferCount(); i++)
		{
			ComPtr<ID3D12Resource> texture;

			swapChain->GetBuffer(i, IID_PPV_ARGS(&texture));

			Core::GraphicsDevice::get()->CreateRenderTargetView(texture.Get(), nullptr, descriptorHandle.getCPUHandle());

			backBufferHandles[i] = descriptorHandle.getCPUHandle();

			descriptorHandle.move();

			backBufferTextures[i] = new Texture(texture, true, D3D12_RESOURCE_STATE_PRESENT);
		}
	}

	if (initializeImGuiSurface)
	{
		LOGENGINE(L"enable ImGui");

		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		(void)io;

		ImGui::StyleColorsDark();

		const DescriptorHandle handle = Core::GlobalDescriptorHeap::getResourceHeap()->allocStaticDescriptor(1);

		ImGui_ImplWin32_Init(hwnd);
		ImGui_ImplDX12_Init(Core::GraphicsDevice::get(), Core::Graphics::getFrameBufferCount(), Core::Graphics::backBufferFormat,
			Core::GlobalDescriptorHeap::getResourceHeap()->get(), handle.getCPUHandle(), handle.getGPUHandle());
	}
	else
	{
		LOGENGINE(L"disable ImGui");
	}

	CHECKERROR(CoInitializeEx(0, COINIT_MULTITHREADED));

	resManager = new ResourceManager();

	createStaticResources();

	submitCommandList(resManager->getCommandList());
}

RenderEnginePrivate::~RenderEnginePrivate()
{
	releaseStaticResources();

	if (resManager)
	{
		delete resManager;
	}

	CoUninitialize();

	if (initializeImGuiSurface)
	{
		ImGui_ImplDX12_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();
	}

	if (backBufferTextures)
	{
		for (uint32_t i = 0; i < Core::Graphics::getFrameBufferCount(); i++)
		{
			delete backBufferTextures[i];
		}

		delete[] backBufferTextures;
	}

	if (backBufferHandles)
	{
		delete[] backBufferHandles;
	}

	if (reservedGlobalCBuffer)
	{
		delete reservedGlobalCBuffer;
	}

	if (prepareCommandList)
	{
		delete prepareCommandList;
	}

	fence = nullptr;

	if (fenceValues)
	{
		delete[] fenceValues;
	}

	swapChain = nullptr;

	Core::DynamicCBufferManager::Internal::release();

	Core::GlobalRootSignature::Internal::release();

	Core::GlobalDescriptorHeap::Internal::release();

	Core::GlobalShader::Internal::release();

	commandQueue = nullptr;

	Core::GraphicsDevice::Internal::release();

	CloseHandle(fenceEvent);
}

void RenderEnginePrivate::submitCommandList(CommandList* const commandList)
{
	std::lock_guard<std::mutex> lockGuard(submitCommandListLock);

	std::vector<D3D12_RESOURCE_BARRIER> barriers;

	commandList->solvePendingBarriers(barriers);

	CommandList* const helperCommandList = recordCommandLists.back();

	if (barriers.size() > 0)
	{
		helperCommandList->resourceBarrier(static_cast<uint32_t>(barriers.size()), barriers.data());
	}

	//不应该关闭准备命令列表，因为要用它来转变后备缓冲的状态，此外还要用它来记录动态常量缓冲更新指令。
	if (helperCommandList != prepareCommandList)
	{
		helperCommandList->close();
	}

	recordCommandLists.push_back(commandList);

	commandList->updateReferredSharedResourceStates();
}

Core::GPUVendor RenderEnginePrivate::getVendor() const
{
	return vendor;
}

Texture* RenderEnginePrivate::getRenderTexture() const
{
	return renderTexture;
}

bool RenderEnginePrivate::getDisplayImGuiSurface() const
{
	return displayImGUISurface;
}

void RenderEnginePrivate::waitForCurrentFrame()
{
	commandQueue->Signal(fence.Get(), fenceValues[Core::Graphics::getFrameIndex()]);

	fence->SetEventOnCompletion(fenceValues[Core::Graphics::getFrameIndex()], fenceEvent);

	WaitForSingleObjectEx(fenceEvent, INFINITE, FALSE);

	fenceValues[Core::Graphics::getFrameIndex()]++;
}

void RenderEnginePrivate::waitForNextFrame()
{
	const uint64_t currentFenceValue = fenceValues[Core::Graphics::getFrameIndex()];

	commandQueue->Signal(fence.Get(), currentFenceValue);

	Core::Graphics::Internal::setFrameIndex(swapChain->GetCurrentBackBufferIndex());

	if (fence->GetCompletedValue() < fenceValues[Core::Graphics::getFrameIndex()])
	{
		fence->SetEventOnCompletion(fenceValues[Core::Graphics::getFrameIndex()], fenceEvent);

		WaitForSingleObjectEx(fenceEvent, INFINITE, FALSE);
	}

	fenceValues[Core::Graphics::getFrameIndex()] = currentFenceValue + 1;
}

void RenderEnginePrivate::begin()
{
	beginImGuiFrame();

	prepareCommandList->open();

	recordCommandLists.push_back(prepareCommandList);

	Core::Graphics::Internal::renderedFrameCountInc();
}

void RenderEnginePrivate::end()
{
	if (displayImGUISurface)
	{
		ImGui::Begin("Frame Profile");
		ImGui::Text("TimeElapsed %.2f", Core::Graphics::getTimeElapsed());
		ImGui::Text("FrameTime %.8f", ImGui::GetIO().DeltaTime * 1000.f);
		ImGui::Text("FrameRate %.1f", ImGui::GetIO().Framerate);
		ImGui::SliderInt("Sync Interval", &syncInterval, 0, 3);
		ImGui::End();
	}

	//把后备缓冲转变到STATE_RENDER_TARGET，并更新动态常量缓冲
	{
		{
			prepareCommandList->pushResourceTrackList(getRenderTexture());

			getRenderTexture()->setAllState(D3D12_RESOURCE_STATE_RENDER_TARGET);
		}

		updateConstantBuffer();

		perframeResource = PerframeResource{
		Core::Graphics::getDeltaTime(),
		Core::Graphics::getTimeElapsed(),
		Utils::Random::genUint(),
		Utils::Random::genFloat(),
		Core::MainCamera::Internal::getMatrices(),
		DirectX::XMFLOAT2(static_cast<float>(Core::Graphics::getWidth()), static_cast<float>(Core::Graphics::getHeight())),
		DirectX::XMFLOAT2(1.f / Core::Graphics::getWidth(), 1.f / Core::Graphics::getHeight()) };

		const StaticCBuffer::UpdateStruct updateStruct = reservedGlobalCBuffer->getUpdateStruct(&perframeResource, sizeof(PerframeResource));

		prepareCommandList->copyBufferRegion(updateStruct.buffer, 0, updateStruct.uploadHeap, 0, sizeof(PerframeResource));

		prepareCommandList->pushResourceTrackList(updateStruct.buffer);

		updateStruct.buffer->setState(D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);

		prepareCommandList->transitionResources();
	}

	//使用最后一个命令列表做些收尾工作
	//如有需要，则绘制ImGui帧
	//把后备缓冲转变到STATE_PRESENT
	{
		CommandList* const finishCommandList = recordCommandLists.back();

		drawImGuiFrame(finishCommandList);

		finishCommandList->pushResourceTrackList(getRenderTexture());

		getRenderTexture()->setAllState(D3D12_RESOURCE_STATE_PRESENT);

		finishCommandList->transitionResources();
	}

	processCommandLists();
}

void RenderEnginePrivate::present() const
{
	swapChain->Present(static_cast<uint32_t>(syncInterval), 0);
}

void RenderEnginePrivate::setDeltaTime(const float deltaTime) const
{
	Core::Graphics::Internal::setDeltaTime(deltaTime);
}

void RenderEnginePrivate::updateTimeElapsed() const
{
	Core::Graphics::Internal::updateTimeElapsed();
}

void RenderEnginePrivate::setDefRenderTexture()
{
	setRenderTexture(backBufferTextures[Core::Graphics::getFrameIndex()], backBufferHandles[Core::Graphics::getFrameIndex()]);
}

void RenderEnginePrivate::setRenderTexture(Texture* const renderTexture, const D3D12_CPU_DESCRIPTOR_HANDLE handle)
{
	//状态转变
	this->renderTexture = renderTexture;

	//渲染目标
	Core::Graphics::Internal::setBackBufferHandle(handle);
}

void RenderEnginePrivate::initializeResources()
{
	//如果有需要，那么开启ImGui
	toggleImGuiSurface();

	//更新动态常量缓冲，因为资源创建可能会需要动态常量缓冲
	updateConstantBuffer();

	processCommandLists();

	//等待准备工作完成
	waitForCurrentFrame();

	//清理静态资源管理器创建的临时资源
	resManager->cleanTransientResources();
}

void RenderEnginePrivate::saveBackBuffer(ReadbackHeap* const readbackHeap)
{
	D3D12_PLACED_SUBRESOURCE_FOOTPRINT bufferFootprint = {};

	bufferFootprint.Footprint.Width = getRenderTexture()->getWidth();

	bufferFootprint.Footprint.Height = getRenderTexture()->getHeight();

	bufferFootprint.Footprint.Depth = 1;

	bufferFootprint.Footprint.RowPitch = Core::FMT::getByteSize(Core::Graphics::backBufferFormat) * getRenderTexture()->getWidth();

	bufferFootprint.Footprint.Format = Core::Graphics::backBufferFormat;

	const CD3DX12_TEXTURE_COPY_LOCATION copyDest(readbackHeap->getResource(), bufferFootprint);

	const CD3DX12_TEXTURE_COPY_LOCATION copySrc(getRenderTexture()->getResource(), 0);

	ID3D12GraphicsCommandList6* const lastCommandList = recordCommandLists.back()->get();

	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = getRenderTexture()->getResource();
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_COPY_SOURCE;

	lastCommandList->ResourceBarrier(1, &barrier);

	lastCommandList->CopyTextureRegion(&copyDest, 0, 0, 0, &copySrc, nullptr);

	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_SOURCE;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;

	lastCommandList->ResourceBarrier(1, &barrier);
}

ComPtr<IDXGIAdapter4> RenderEnginePrivate::getBestAdapterAndVendor(IDXGIFactory7* const factory)
{
	ComPtr<IDXGIAdapter4> adapter;

	for (uint32_t adapterIndex = 0;
		SUCCEEDED(factory->EnumAdapterByGpuPreference(adapterIndex, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&adapter)));
		adapterIndex++)
	{
		DXGI_ADAPTER_DESC3 desc = {};

		adapter->GetDesc3(&desc);

		if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
		{
			continue;
		}

		if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), nullptr)))
		{
			const uint32_t vendorID = desc.VendorId;

			std::wstring vendorName;

			if (vendorID == 0x10DE)
			{
				vendor = Core::GPUVendor::NVIDIA;

				vendorName = L"NVIDIA";
			}
			else if (vendorID == 0x1002 || vendorID == 0x1022)
			{
				vendor = Core::GPUVendor::AMD;

				vendorName = L"AMD";
			}
			else if (vendorID == 0x163C || vendorID == 0x8086 || vendorID == 0x8087)
			{
				vendor = Core::GPUVendor::INTEL;

				vendorName = L"INTEL";
			}
			else
			{
				vendor = Core::GPUVendor::UNKNOWN;

				vendorName = L"UNKNOWN";
			}

			LOGENGINE(L"following are information about selected GPU");

			LOGENGINE(L"GPU Name", LogColor::brightMagenta, desc.Description);

			LOGENGINE(L"GPU Vendor ID", IntegerMode::HEX, vendorID);

			LOGENGINE(L"GPU Vendor Name", LogColor::brightMagenta, vendorName);

			LOGENGINE(L"GPU Dedicated Memory", static_cast<float>(desc.DedicatedVideoMemory) / 1024.f / 1024.f / 1024.f, L"gigabytes");

			break;
		}
	}

	return adapter;
}

void RenderEnginePrivate::processCommandLists()
{
	recordCommandLists.front()->close();

	recordCommandLists.back()->close();

	std::vector<ID3D12CommandList*> commandLists;

	for (const CommandList* const commandList : recordCommandLists)
	{
		commandLists.push_back(commandList->get());
	}

	recordCommandLists.clear();

	commandQueue->ExecuteCommandLists(static_cast<uint32_t>(commandLists.size()), commandLists.data());
}

void RenderEnginePrivate::updateConstantBuffer() const
{
	Core::DynamicCBufferManager::Internal::recordCommands(prepareCommandList);
}

void RenderEnginePrivate::toggleImGuiSurface()
{
	if (initializeImGuiSurface)
	{
		displayImGUISurface = !displayImGUISurface;
	}
}

void RenderEnginePrivate::beginImGuiFrame() const
{
	if (displayImGUISurface)
	{
		ImGui_ImplDX12_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
	}
}

void RenderEnginePrivate::drawImGuiFrame(CommandList* const targetCommandList)
{
	if (displayImGUISurface)
	{
		ImGui::Render();

		//这里假设了全局描述堆和全局采样器堆已经进行了绑定
		//targetCommandList->setDescriptorHeap(Core::GlobalDescriptorHeap::getResourceHeap(), Core::GlobalDescriptorHeap::getSamplerHeap());

		targetCommandList->setDefRenderTarget();

		ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), targetCommandList->get());
	}
}

void RenderEnginePrivate::createStaticResources()
{
	GraphicsContext* const context = resManager->getGraphicsContext();

	context->begin();

	Core::StaticEffect::BackBufferBlitEffect::Internal::initialize();

	Core::StaticEffect::HDRClampEffect::Internal::initialize();

	Core::StaticEffect::LatLongMapToCubeMapEffect::Internal::initialize(resManager);
}

void RenderEnginePrivate::releaseStaticResources()
{
	Core::StaticEffect::BackBufferBlitEffect::Internal::release();

	Core::StaticEffect::HDRClampEffect::Internal::release();

	Core::StaticEffect::LatLongMapToCubeMapEffect::Internal::release();
}

void Core::RenderEngine::submitCommandList(CommandList* const commandList)
{
	pvt->submitCommandList(commandList);
}

Core::GPUVendor Core::RenderEngine::getVendor()
{
	return pvt->getVendor();
}

Texture* Core::RenderEngine::getRenderTexture()
{
	return pvt->getRenderTexture();
}

bool Core::RenderEngine::getDisplayImGuiSurface()
{
	return pvt->getDisplayImGuiSurface();
}

void Core::RenderEngine::Internal::initialize(const uint32_t width, const uint32_t height, const HWND hwnd, const bool useSwapChainBuffer, const bool initializeImGuiSurface)
{
	pvt = new RenderEnginePrivate(width, height, hwnd, useSwapChainBuffer, initializeImGuiSurface);
}

void Core::RenderEngine::Internal::release()
{
	if (pvt)
	{
		delete pvt;
	}
}

void Core::RenderEngine::Internal::waitForCurrentFrame()
{
	pvt->waitForCurrentFrame();
}

void Core::RenderEngine::Internal::waitForNextFrame()
{
	pvt->waitForNextFrame();
}

void Core::RenderEngine::Internal::begin()
{
	pvt->begin();
}

void Core::RenderEngine::Internal::end()
{
	pvt->end();
}

void Core::RenderEngine::Internal::present()
{
	pvt->present();
}

void Core::RenderEngine::Internal::setDeltaTime(const float deltaTime)
{
	pvt->setDeltaTime(deltaTime);
}

void Core::RenderEngine::Internal::updateTimeElapsed()
{
	pvt->updateTimeElapsed();
}

void Core::RenderEngine::Internal::saveBackBuffer(ReadbackHeap* const readbackHeap)
{
	pvt->saveBackBuffer(readbackHeap);
}

void Core::RenderEngine::Internal::setDefRenderTexture()
{
	pvt->setDefRenderTexture();
}

void Core::RenderEngine::Internal::setRenderTexture(Texture* const renderTexture, const D3D12_CPU_DESCRIPTOR_HANDLE handle)
{
	pvt->setRenderTexture(renderTexture, handle);
}

void Core::RenderEngine::Internal::initializeResources()
{
	pvt->initializeResources();
}