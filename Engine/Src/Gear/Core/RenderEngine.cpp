#include<Gear/Core/RenderEngine.h>

RenderEngine* RenderEngine::instance = nullptr;

RenderEngine* RenderEngine::get()
{
	return instance;
}

void RenderEngine::submitCommandList(CommandList* const commandList)
{
	std::lock_guard<std::mutex> lockGuard(submitCommandListLock);

	std::vector<D3D12_RESOURCE_BARRIER> barriers;

	{
		for (const PendingBufferBarrier& pendingBarrier : commandList->pendingBufferBarrier)
		{
			if ((*(pendingBarrier.buffer->globalState)) != pendingBarrier.afterState)
			{
				D3D12_RESOURCE_BARRIER barrier = {};
				barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
				barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
				barrier.Transition.pResource = pendingBarrier.buffer->getResource();
				barrier.Transition.StateBefore = static_cast<D3D12_RESOURCE_STATES>(*(pendingBarrier.buffer->globalState));
				barrier.Transition.StateAfter = static_cast<D3D12_RESOURCE_STATES>(pendingBarrier.afterState);
				barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

				barriers.push_back(barrier);
			}
		}

		commandList->pendingBufferBarrier.clear();
	}

	{
		for (const PendingTextureBarrier& pendingBarrier : commandList->pendingTextureBarrier)
		{
			if (pendingBarrier.mipSlice == D3D12_TRANSITION_ALL_MIPLEVELS)
			{
				if (pendingBarrier.texture->mipLevels > 1)
				{
					bool uniformState = true;

					if ((*(pendingBarrier.texture->globalState)).allState == D3D12_RESOURCE_STATE_UNKNOWN)
					{
						const UINT tempState = (*(pendingBarrier.texture->globalState)).mipLevelStates[0];

						for (UINT mipSlice = 0; mipSlice < pendingBarrier.texture->mipLevels; mipSlice++)
						{
							if ((*(pendingBarrier.texture->globalState)).mipLevelStates[mipSlice] != tempState)
							{
								uniformState = false;
								break;
							}
						}
					}

					if (uniformState)
					{
						if ((*(pendingBarrier.texture->globalState)).mipLevelStates[0] != pendingBarrier.afterState)
						{
							D3D12_RESOURCE_BARRIER barrier = {};
							barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
							barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
							barrier.Transition.pResource = pendingBarrier.texture->getResource();
							barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
							barrier.Transition.StateBefore = static_cast<D3D12_RESOURCE_STATES>((*(pendingBarrier.texture->globalState)).mipLevelStates[0]);
							barrier.Transition.StateAfter = static_cast<D3D12_RESOURCE_STATES>(pendingBarrier.afterState);

							barriers.push_back(barrier);
						}
					}
					else
					{
						for (UINT mipSlice = 0; mipSlice < pendingBarrier.texture->mipLevels; mipSlice++)
						{
							if ((*(pendingBarrier.texture->globalState)).mipLevelStates[mipSlice] != pendingBarrier.afterState)
							{
								for (UINT arraySlice = 0; arraySlice < pendingBarrier.texture->arraySize; arraySlice++)
								{
									D3D12_RESOURCE_BARRIER barrier = {};
									barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
									barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
									barrier.Transition.pResource = pendingBarrier.texture->getResource();
									barrier.Transition.Subresource = D3D12CalcSubresource(mipSlice, arraySlice, 0, pendingBarrier.texture->mipLevels, pendingBarrier.texture->arraySize);
									barrier.Transition.StateBefore = static_cast<D3D12_RESOURCE_STATES>((*(pendingBarrier.texture->globalState)).mipLevelStates[mipSlice]);
									barrier.Transition.StateAfter = static_cast<D3D12_RESOURCE_STATES>(pendingBarrier.afterState);

									barriers.push_back(barrier);
								}
							}
						}
					}
				}
				else
				{
					if ((*(pendingBarrier.texture->globalState)).allState != pendingBarrier.afterState)
					{
						D3D12_RESOURCE_BARRIER barrier = {};
						barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
						barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
						barrier.Transition.pResource = pendingBarrier.texture->getResource();
						barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
						barrier.Transition.StateBefore = static_cast<D3D12_RESOURCE_STATES>((*(pendingBarrier.texture->globalState)).allState);
						barrier.Transition.StateAfter = static_cast<D3D12_RESOURCE_STATES>(pendingBarrier.afterState);

						barriers.push_back(barrier);
					}
				}
			}
			else
			{
				if (pendingBarrier.texture->mipLevels > 1)
				{
					if ((*(pendingBarrier.texture->globalState)).mipLevelStates[pendingBarrier.mipSlice] != pendingBarrier.afterState)
					{
						for (UINT arraySlice = 0; arraySlice < pendingBarrier.texture->arraySize; arraySlice++)
						{
							D3D12_RESOURCE_BARRIER barrier = {};
							barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
							barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
							barrier.Transition.pResource = pendingBarrier.texture->getResource();
							barrier.Transition.Subresource = D3D12CalcSubresource(pendingBarrier.mipSlice, arraySlice, 0, pendingBarrier.texture->mipLevels, pendingBarrier.texture->arraySize);
							barrier.Transition.StateBefore = static_cast<D3D12_RESOURCE_STATES>((*(pendingBarrier.texture->globalState)).mipLevelStates[pendingBarrier.mipSlice]);
							barrier.Transition.StateAfter = static_cast<D3D12_RESOURCE_STATES>(pendingBarrier.afterState);

							barriers.push_back(barrier);
						}
					}
				}
				else
				{
					throw "Transition texture with only 1 miplevel however its pending mipslice is not D3D12_TRANSITION_ALL_MIPLEVELS is not allowed!\n";
					//in this case target mipslice is not D3D12_TRANSITION_ALL_MIPLEVELS for texture has only 1 miplevel
				}
			}
		}

		commandList->pendingTextureBarrier.clear();
	}

	CommandList* const helperCommandList = recordCommandLists.back();

	if (barriers.size() > 0)
	{
		helperCommandList->resourceBarrier(static_cast<UINT>(barriers.size()), barriers.data());
	}

	//we should not close prepareCommandList 
	if (helperCommandList != prepareCommandList)
	{
		helperCommandList->close();
	}

	recordCommandLists.push_back(commandList);

	commandList->updateReferredResStates();
}

void RenderEngine::processCommandLists()
{
	recordCommandLists.front()->close();

	recordCommandLists.back()->close();

	std::vector<ID3D12CommandList*> commandLists;

	for (CommandList* const commandList : recordCommandLists)
	{
		commandLists.push_back(commandList->get());
	}

	recordCommandLists.clear();

	commandQueue->ExecuteCommandLists(static_cast<UINT>(commandLists.size()), commandLists.data());
}

void RenderEngine::waitForPreviousFrame()
{
	commandQueue->Signal(fence.Get(), fenceValues[Graphics::frameIndex]);

	fence->SetEventOnCompletion(fenceValues[Graphics::frameIndex], fenceEvent);

	WaitForSingleObjectEx(fenceEvent, INFINITE, FALSE);

	fenceValues[Graphics::frameIndex]++;
}

void RenderEngine::waitForNextFrame()
{
	swapChain->Present(1, 0);

	const UINT64 currentFenceValue = fenceValues[Graphics::frameIndex];

	commandQueue->Signal(fence.Get(), currentFenceValue);

	Graphics::frameIndex = swapChain->GetCurrentBackBufferIndex();

	if (fence->GetCompletedValue() < fenceValues[Graphics::frameIndex])
	{
		fence->SetEventOnCompletion(fenceValues[Graphics::frameIndex], fenceEvent);

		WaitForSingleObjectEx(fenceEvent, INFINITE, FALSE);
	}

	fenceValues[Graphics::frameIndex] = currentFenceValue + 1;
}

void RenderEngine::begin()
{
	beginImGuiFrame();

	prepareCommandList->open();

	recordCommandLists.push_back(prepareCommandList);
}

void RenderEngine::end()
{
	if (displayImGUISurface)
	{
		ImGui::Begin("Frame Profile");
		ImGui::Text("TimeElapsed %.2f", Graphics::getTimeElapsed());
		ImGui::Text("FrameTime %.8f", ImGui::GetIO().DeltaTime * 1000.f);
		ImGui::Text("FrameRate %.1f", ImGui::GetIO().Framerate);
		ImGui::End();
	}

	//transition back buffer to STATE_RENDER_TARGET
	//update all constant buffers
	{
		{
			prepareCommandList->pushResourceTrackList(getCurrentRenderTexture());

			getCurrentRenderTexture()->setAllState(D3D12_RESOURCE_STATE_RENDER_TARGET);
		}

		perFrameResource.time = Graphics::time;

		perFrameResource.matrices = Camera::matrices;

		perFrameResource.screenSize = DirectX::XMFLOAT2(static_cast<float>(Graphics::getWidth()), static_cast<float>(Graphics::getHeight()));

		perFrameResource.screenTexelSize = DirectX::XMFLOAT2(1.f / Graphics::getWidth(), 1.f / Graphics::getHeight());

		GraphicsContext::globalConstantBuffer->update(&perFrameResource, sizeof(PerFrameResource));

		updateConstantBuffer();
	}

	//we rely on the last commandList to do some finishing works
	//draw ImGui frame 
	//transition back buffer to STATE_PRESENT
	{
		CommandList* const finishCommandList = recordCommandLists.back();

		drawImGuiFrame(finishCommandList);

		finishCommandList->pushResourceTrackList(getCurrentRenderTexture());

		getCurrentRenderTexture()->setAllState(D3D12_RESOURCE_STATE_PRESENT);

		finishCommandList->transitionResources();
	}

	processCommandLists();
}

void RenderEngine::updateConstantBuffer()
{
	ConstantBufferManager::get()->recordCommands(prepareCommandList);
}

GPUVendor RenderEngine::getVendor() const
{
	return vendor;
}

Texture* RenderEngine::getCurrentRenderTexture() const
{
	return backBufferResources[Graphics::getFrameIndex()];
}

bool RenderEngine::getDisplayImGuiSurface() const
{
	return displayImGUISurface;
}

ComPtr<IDXGIAdapter4> RenderEngine::getBestAdapterAndVendor(IDXGIFactory7* const factory)
{
	ComPtr<IDXGIAdapter4> adapter;

	for (UINT adapterIndex = 0;
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
			const UINT vendorID = desc.VendorId;

			std::cout << "[class RenderEngine] GPU VendorID 0x" << std::hex << vendorID << std::dec << " Vendor:";

			if (vendorID == 0x10DE)
			{
				std::cout << "NVIDIA\n";
				vendor = GPUVendor::NVIDIA;
			}
			else if (vendorID == 0x1002 || vendorID == 0x1022)
			{
				std::cout << "AMD\n";
				vendor = GPUVendor::AMD;
			}
			else if (vendorID == 0x163C || vendorID == 0x8086 || vendorID == 0x8087)
			{
				std::cout << "INTEL\n";
				vendor = GPUVendor::INTEL;
			}
			else
			{
				std::cout << "UNKNOWN\n";
				vendor = GPUVendor::UNKNOWN;
			}

			break;
		}
	}

	return adapter;
}

void RenderEngine::initializeResources()
{
	//turn on ImGui surface if it is needed
	toggleImGuiSurface();

	//update all constant buffers since resource creation might need these buffers
	updateConstantBuffer();

	processCommandLists();

	//wait little seconds here
	waitForPreviousFrame();
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

		//we take an assumption here that global descriptor heap and global sampler heap are bound on target command list by default
		//targetCommandList->setDescriptorHeap(GlobalDescriptorHeap::getResourceHeap(), GlobalDescriptorHeap::getSamplerHeap());

		targetCommandList->setDefRenderTarget();

		ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), targetCommandList->get());
	}
}

RenderEngine::RenderEngine(const HWND hwnd, const bool useSwapChainBuffer, const bool initializeImGuiSurface) :
	fenceValues{}, fenceEvent(nullptr), vendor(GPUVendor::UNKNOWN), perFrameResource{}, initializeImGuiSurface(initializeImGuiSurface), displayImGUISurface(false)
{
	ComPtr<IDXGIFactory7> factory;

#ifdef _DEBUG
	std::cout << "[class RenderEngine] enable debug layer\n";

	ComPtr<ID3D12Debug> debugController;

	D3D12GetDebugInterface(IID_PPV_ARGS(&debugController));

	debugController->EnableDebugLayer();

	CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, IID_PPV_ARGS(&factory));
#else
	std::cout << "[class RenderEngine] disable debug layer\n";

	CreateDXGIFactory2(0, IID_PPV_ARGS(&factory));
#endif // _DEBUG

	ComPtr<IDXGIAdapter4> adapter = getBestAdapterAndVendor(factory.Get());

	GraphicsDevice::instance = new GraphicsDevice(adapter.Get());

	GraphicsDevice::instance->checkFeatureSupport();

	{
		D3D12_COMMAND_QUEUE_DESC queueDesc = {};
		queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

		GraphicsDevice::get()->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&commandQueue));
	}

	Shader::createGlobalShaders();

	States::initialize();

	GlobalDescriptorHeap::instance = new GlobalDescriptorHeap();

	GlobalRootSignature::instance = new GlobalRootSignature();

	ConstantBufferManager::instance = new ConstantBufferManager();

	PipelineState::instance = new PipelineState();

	{
		DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
		swapChainDesc.BufferCount = Graphics::FrameBufferCount;
		swapChainDesc.Width = Graphics::getWidth();
		swapChainDesc.Height = Graphics::getHeight();
		swapChainDesc.Format = Graphics::BackBufferFormat;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		swapChainDesc.SampleDesc.Count = 1;
		swapChainDesc.SampleDesc.Quality = 0;

		ComPtr<IDXGISwapChain1> swapChain1;

		factory->CreateSwapChainForHwnd(commandQueue.Get(), hwnd, &swapChainDesc, nullptr, nullptr, &swapChain1);

		factory->MakeWindowAssociation(hwnd, DXGI_MWA_NO_ALT_ENTER);

		swapChain1.As(&swapChain);
	}

	GraphicsDevice::get()->CreateFence(fenceValues[Graphics::frameIndex], D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));

	fenceValues[Graphics::frameIndex]++;

	fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);

	prepareCommandList = new CommandList(D3D12_COMMAND_LIST_TYPE_DIRECT);

	GraphicsContext::globalConstantBuffer = ResourceManager::createConstantBuffer(sizeof(PerFrameResource), true);

	//make sure beginCommandList is always the first element of recordCommandLists
	//and we may need constant buffer for resource creation 
	begin();

	{
		DescriptorHandle descriptorHandle = GlobalDescriptorHeap::getRenderTargetHeap()->allocStaticDescriptor(Graphics::FrameBufferCount);

		if (useSwapChainBuffer)
		{
			for (UINT i = 0; i < Graphics::FrameBufferCount; i++)
			{
				ComPtr<ID3D12Resource> texture;

				swapChain->GetBuffer(i, IID_PPV_ARGS(&texture));

				GraphicsDevice::get()->CreateRenderTargetView(texture.Get(), nullptr, descriptorHandle.getCPUHandle());

				Graphics::backBufferHandles[i] = descriptorHandle.getCPUHandle();

				descriptorHandle.move();

				backBufferResources[i] = new Texture(texture, true, D3D12_RESOURCE_STATE_PRESENT);
			}
		}
		else
		{
			D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};
			rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
			rtvDesc.Format = Graphics::BackBufferFormat;
			rtvDesc.Texture2D.MipSlice = 0;
			rtvDesc.Texture2D.PlaneSlice = 0;

			for (UINT i = 0; i < Graphics::FrameBufferCount; i++)
			{
				ComPtr<ID3D12Resource> texture;

				const CD3DX12_HEAP_PROPERTIES heapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);

				const CD3DX12_RESOURCE_DESC resourceDesc = CD3DX12_RESOURCE_DESC::Tex2D(Graphics::BackBufferFormat, Graphics::width, Graphics::height, 1, 1, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET);

				const D3D12_CLEAR_VALUE clearValue = { Graphics::BackBufferFormat ,{0.f,0.f,0.f,1.f} };

				GraphicsDevice::get()->CreateCommittedResource(&heapProperties, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_PRESENT, &clearValue, IID_PPV_ARGS(&texture));

				GraphicsDevice::get()->CreateRenderTargetView(texture.Get(), &rtvDesc, descriptorHandle.getCPUHandle());

				Graphics::backBufferHandles[i] = descriptorHandle.getCPUHandle();

				descriptorHandle.move();

				backBufferResources[i] = new Texture(texture, true, D3D12_RESOURCE_STATE_PRESENT);
			}
		}
	}

	if (initializeImGuiSurface)
	{
		std::cout << "[class RenderEngine] enable ImGui\n";

		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		(void)io;

		ImGui::StyleColorsDark();

		const DescriptorHandle handle = GlobalDescriptorHeap::getResourceHeap()->allocStaticDescriptor(1);

		ImGui_ImplWin32_Init(hwnd);
		ImGui_ImplDX12_Init(GraphicsDevice::get(), Graphics::FrameBufferCount, Graphics::BackBufferFormat,
			GlobalDescriptorHeap::getResourceHeap()->get(), handle.getCPUHandle(), handle.getGPUHandle());
	}
	else
	{
		std::cout << "[class RenderEngine] disable ImGui\n";
	}
}

RenderEngine::~RenderEngine()
{
	if (initializeImGuiSurface)
	{
		ImGui_ImplDX12_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();
	}

	if (GraphicsContext::globalConstantBuffer)
	{
		delete GraphicsContext::globalConstantBuffer;
	}

	if (GlobalDescriptorHeap::instance)
	{
		delete GlobalDescriptorHeap::instance;
	}

	if (GlobalRootSignature::instance)
	{
		delete GlobalRootSignature::instance;
	}

	if (ConstantBufferManager::instance)
	{
		delete ConstantBufferManager::instance;
	}

	if (PipelineState::instance)
	{
		delete PipelineState::instance;
	}

	Shader::releaseGlobalShaders();

	if (prepareCommandList)
	{
		delete prepareCommandList;
	}

	for (UINT i = 0; i < Graphics::FrameBufferCount; i++)
	{
		delete backBufferResources[i];
	}
}