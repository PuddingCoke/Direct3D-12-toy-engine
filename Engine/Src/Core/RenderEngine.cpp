#include<Gear/Core/RenderEngine.h>

#include<Gear/Core/Graphics.h>

#include<Gear/Core/Internal/GraphicsInternal.h>

#include<Gear/Core/Shader.h>

#include<Gear/Core/PipelineState.h>

#include<Gear/Core/DynamicCBufferManager.h>

#include<Gear/Core/GlobalRootSignature.h>

#include<Gear/Core/StaticResourceManager.h>

#include<Gear/Utils/Random.h>

RenderEngine* RenderEngine::instance = nullptr;

RenderEngine* RenderEngine::get()
{
	return instance;
}

void RenderEngine::submitCommandList(CommandList* const commandList)
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

void RenderEngine::processCommandLists()
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

void RenderEngine::waitForCurrentFrame()
{
	commandQueue->Signal(fence.Get(), fenceValues[Graphics::getFrameIndex()]);

	fence->SetEventOnCompletion(fenceValues[Graphics::getFrameIndex()], fenceEvent);

	WaitForSingleObjectEx(fenceEvent, INFINITE, FALSE);

	fenceValues[Graphics::getFrameIndex()]++;
}

void RenderEngine::waitForNextFrame()
{
	const uint64_t currentFenceValue = fenceValues[Graphics::getFrameIndex()];

	commandQueue->Signal(fence.Get(), currentFenceValue);

	Graphics::Internal::setFrameIndex(swapChain->GetCurrentBackBufferIndex());

	if (fence->GetCompletedValue() < fenceValues[Graphics::getFrameIndex()])
	{
		fence->SetEventOnCompletion(fenceValues[Graphics::getFrameIndex()], fenceEvent);

		WaitForSingleObjectEx(fenceEvent, INFINITE, FALSE);
	}

	fenceValues[Graphics::getFrameIndex()] = currentFenceValue + 1;
}

void RenderEngine::begin()
{
	beginImGuiFrame();

	prepareCommandList->open();

	recordCommandLists.push_back(prepareCommandList);

	Graphics::Internal::renderedFrameCountInc();
}

void RenderEngine::end()
{
	if (displayImGUISurface)
	{
		ImGui::Begin("Frame Profile");
		ImGui::Text("TimeElapsed %.2f", Graphics::getTimeElapsed());
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

		perFrameResource.deltaTime = Graphics::getDeltaTime();

		perFrameResource.timeElapsed = Graphics::getTimeElapsed();

		perFrameResource.uintSeed = Utils::Random::genUint();

		perFrameResource.floatSeed = Utils::Random::genFloat();

		perFrameResource.matrices = Camera::matrices;

		perFrameResource.screenSize = DirectX::XMFLOAT2(static_cast<float>(Graphics::getWidth()), static_cast<float>(Graphics::getHeight()));

		perFrameResource.screenTexelSize = DirectX::XMFLOAT2(1.f / Graphics::getWidth(), 1.f / Graphics::getHeight());

		reservedGlobalCBuffer->update(&perFrameResource);

		updateConstantBuffer();
	}

	//使用最后一个命令列表做些收尾工作
	//如有需要绘制ImGui帧
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

void RenderEngine::present() const
{
	swapChain->Present(static_cast<uint32_t>(syncInterval), 0);
}

void RenderEngine::updateConstantBuffer() const
{
	DynamicCBufferManager::get()->recordCommands(prepareCommandList);
}

GPUVendor RenderEngine::getVendor() const
{
	return vendor;
}

Texture* RenderEngine::getRenderTexture() const
{
	return renderTexture;
}

bool RenderEngine::getDisplayImGuiSurface() const
{
	return displayImGUISurface;
}

ComPtr<IDXGIAdapter4> RenderEngine::getBestAdapterAndVendor(IDXGIFactory7* const factory)
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
				vendor = GPUVendor::NVIDIA;

				vendorName = L"NVIDIA";
			}
			else if (vendorID == 0x1002 || vendorID == 0x1022)
			{
				vendor = GPUVendor::AMD;

				vendorName = L"AMD";
			}
			else if (vendorID == 0x163C || vendorID == 0x8086 || vendorID == 0x8087)
			{
				vendor = GPUVendor::INTEL;

				vendorName = L"INTEL";
			}
			else
			{
				vendor = GPUVendor::UNKNOWN;

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

void RenderEngine::initializeResources()
{
	//如果有需要那么开启ImGui
	toggleImGuiSurface();

	//更新动态常量缓冲，因为资源创建可能会需要动态常量缓冲
	updateConstantBuffer();

	processCommandLists();

	//等待准备工作完成
	waitForCurrentFrame();

	//清理静态资源管理器创建的临时资源
	StaticResourceManager::get()->cleanTransientResources();
}

void RenderEngine::toggleImGuiSurface()
{
	if (initializeImGuiSurface)
	{
		displayImGUISurface = !displayImGUISurface;
	}
}

void RenderEngine::beginImGuiFrame() const
{
	if (displayImGUISurface)
	{
		ImGui_ImplDX12_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
	}
}

void RenderEngine::drawImGuiFrame(CommandList* const targetCommandList)
{
	if (displayImGUISurface)
	{
		ImGui::Render();

		//这里假设了全局描述堆和全局采样器堆已经进行了绑定
		//targetCommandList->setDescriptorHeap(GlobalDescriptorHeap::getResourceHeap(), GlobalDescriptorHeap::getSamplerHeap());

		targetCommandList->setDefRenderTarget();

		ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), targetCommandList->get());
	}
}

void RenderEngine::setDefRenderTexture()
{
	setRenderTexture(backBufferTextures[Graphics::getFrameIndex()], backBufferHandles[Graphics::getFrameIndex()]);
}

void RenderEngine::setRenderTexture(Texture* const renderTexture, const D3D12_CPU_DESCRIPTOR_HANDLE handle)
{
	//状态转变
	this->renderTexture = renderTexture;

	//渲染目标
	CommandList::setBackBufferHandle(handle);
}

void RenderEngine::setDeltaTime(const float deltaTime) const
{
	Graphics::Internal::setDeltaTime(deltaTime);
}

void RenderEngine::updateTimeElapsed() const
{
	Graphics::Internal::updateTimeElapsed();
}

void RenderEngine::saveBackBuffer(ReadbackHeap* const readbackHeap)
{
	D3D12_PLACED_SUBRESOURCE_FOOTPRINT bufferFootprint = {};

	bufferFootprint.Footprint.Width = getRenderTexture()->getWidth();

	bufferFootprint.Footprint.Height = getRenderTexture()->getHeight();

	bufferFootprint.Footprint.Depth = 1;

	bufferFootprint.Footprint.RowPitch = FMT::getByteSize(Graphics::backBufferFormat) * getRenderTexture()->getWidth();

	bufferFootprint.Footprint.Format = Graphics::backBufferFormat;

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

RenderEngine::RenderEngine(const uint32_t width, const uint32_t height, const HWND hwnd, const bool useSwapChainBuffer, const bool initializeImGuiSurface) :
	backBufferTextures(nullptr),
	backBufferHandles(nullptr),
	fenceEvent(CreateEvent(nullptr, FALSE, FALSE, nullptr)),
	vendor(GPUVendor::UNKNOWN), perFrameResource{},
	initializeImGuiSurface(initializeImGuiSurface),
	displayImGUISurface(false),
	syncInterval(1)
{
	Graphics::Internal::initialize(useSwapChainBuffer ? 3 : 1, width, height);

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

	GraphicsDevice::instance = new GraphicsDevice(adapter.Get());

	GraphicsDevice::instance->checkFeatureSupport();

	{
		D3D12_COMMAND_QUEUE_DESC queueDesc = {};
		queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_DISABLE_GPU_TIMEOUT;

		GraphicsDevice::get()->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&commandQueue));

		commandQueue->SetName(L"Graphics Command Queue");
	}

	Shader::createGlobalShaders();

	GlobalDescriptorHeap::instance = new GlobalDescriptorHeap();

	GlobalRootSignature::instance = new GlobalRootSignature();

	DynamicCBufferManager::instance = new DynamicCBufferManager();

	{
		DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
		swapChainDesc.BufferCount = useSwapChainBuffer ? Graphics::getFrameBufferCount() : 2;
		swapChainDesc.Width = Graphics::getWidth();
		swapChainDesc.Height = Graphics::getHeight();
		swapChainDesc.Format = Graphics::backBufferFormat;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		swapChainDesc.SampleDesc.Count = 1;
		swapChainDesc.SampleDesc.Quality = 0;

		ComPtr<IDXGISwapChain1> swapChain1;

		factory->CreateSwapChainForHwnd(commandQueue.Get(), hwnd, &swapChainDesc, nullptr, nullptr, &swapChain1);

		factory->MakeWindowAssociation(hwnd, DXGI_MWA_NO_ALT_ENTER);

		swapChain1.As(&swapChain);
	}

	fenceValues = new uint64_t[Graphics::getFrameBufferCount()];

	for (uint32_t i = 0; i < Graphics::getFrameBufferCount(); i++)
	{
		fenceValues[i] = 0;
	}

	GraphicsDevice::get()->CreateFence(fenceValues[Graphics::getFrameIndex()], D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));

	fenceValues[Graphics::getFrameIndex()]++;

	prepareCommandList = new CommandList(D3D12_COMMAND_LIST_TYPE_DIRECT);

	reservedGlobalCBuffer = ResourceManager::createDynamicCBuffer(sizeof(PerFrameResource));

	GraphicsContext::reservedGlobalCBuffer = reservedGlobalCBuffer;

	//确保准备命令列表总是处于容器第一个位置
	begin();

	if (useSwapChainBuffer)
	{
		DescriptorHandle descriptorHandle = GlobalDescriptorHeap::getRenderTargetHeap()->allocStaticDescriptor(Graphics::getFrameBufferCount());

		backBufferTextures = new Texture * [Graphics::getFrameBufferCount()];

		backBufferHandles = new D3D12_CPU_DESCRIPTOR_HANDLE[Graphics::getFrameBufferCount()];

		for (uint32_t i = 0; i < Graphics::getFrameBufferCount(); i++)
		{
			ComPtr<ID3D12Resource> texture;

			swapChain->GetBuffer(i, IID_PPV_ARGS(&texture));

			GraphicsDevice::get()->CreateRenderTargetView(texture.Get(), nullptr, descriptorHandle.getCPUHandle());

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

		const DescriptorHandle handle = GlobalDescriptorHeap::getResourceHeap()->allocStaticDescriptor(1);

		ImGui_ImplWin32_Init(hwnd);
		ImGui_ImplDX12_Init(GraphicsDevice::get(), Graphics::getFrameBufferCount(), Graphics::backBufferFormat,
			GlobalDescriptorHeap::getResourceHeap()->get(), handle.getCPUHandle(), handle.getGPUHandle());
	}
	else
	{
		LOGENGINE(L"disable ImGui");
	}

	CHECKERROR(CoInitializeEx(0, COINIT_MULTITHREADED));

	StaticResourceManager::instance = new StaticResourceManager();

	submitCommandList(StaticResourceManager::get()->commandList);
}

RenderEngine::~RenderEngine()
{
	if (StaticResourceManager::instance)
	{
		delete StaticResourceManager::instance;
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
		for (uint32_t i = 0; i < Graphics::getFrameBufferCount(); i++)
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

	if (DynamicCBufferManager::instance)
	{
		delete DynamicCBufferManager::instance;
	}

	if (GlobalRootSignature::instance)
	{
		delete GlobalRootSignature::instance;
	}

	if (GlobalDescriptorHeap::instance)
	{
		delete GlobalDescriptorHeap::instance;
	}

	Shader::releaseGlobalShaders();

	commandQueue = nullptr;

	if (GraphicsDevice::instance)
	{
		delete GraphicsDevice::instance;
	}

	CloseHandle(fenceEvent);
}