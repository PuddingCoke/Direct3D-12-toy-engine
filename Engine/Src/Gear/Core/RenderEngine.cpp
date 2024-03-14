#include<Gear/Core/RenderEngine.h>

RenderEngine* RenderEngine::instance = nullptr;

RenderEngine* RenderEngine::get()
{
	return instance;
}

void RenderEngine::submitRenderPass(RenderPass* const pass)
{
	std::vector<D3D12_RESOURCE_BARRIER> barriers;

	{
		std::vector<PendingBufferBarrier>* pendingBufferBarrier = &(pass->context->pendingBufferBarrier);

		for (UINT bufferIdx = 0; bufferIdx < pendingBufferBarrier->size(); bufferIdx++)
		{
			const PendingBufferBarrier pendingBarrier = (*pendingBufferBarrier)[bufferIdx];

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

		pendingBufferBarrier->clear();
	}

	{
		std::vector<PendingTextureBarrier>* pendingTextureBarrier = &(pass->context->pendingTextureBarrier);

		for (UINT textureIdx = 0; textureIdx < pendingTextureBarrier->size(); textureIdx++)
		{
			const PendingTextureBarrier pendingBarrier = (*pendingTextureBarrier)[textureIdx];

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
					//but for transition texture only has 1 miplevel target mipslice should be D3D12_TRANSITION_ALL_MIPLEVELS
					//so not handling this case
				}
			}
		}

		pendingTextureBarrier->clear();
	}

	if (barriers.size() > 0)
	{
		pass->transitionCMD->reset();

		pass->transitionCMD->get()->ResourceBarrier(barriers.size(), barriers.data());

		pass->transitionCMD->get()->Close();

		commandLists.push_back(pass->transitionCMD->get());
		commandLists.push_back(pass->context->commandList->get());
	}
	else
	{
		commandLists.push_back(pass->context->commandList->get());
	}

	pass->context->updateReferredResStates();
}

void RenderEngine::processCommandLists()
{
	if (commandLists.size())
	{
		commandQueue->ExecuteCommandLists(commandLists.size(), commandLists.data());

		commandLists.clear();
	}
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
	beginCommandlist->reset();

	commandLists.push_back(beginCommandlist->get());

	{
		perFrameResource.time = Graphics::time;

		perFrameResource.matrices = Camera::matrices;

		perFrameResource.screenSize = DirectX::XMFLOAT2(Graphics::getWidth(), Graphics::getHeight());

		perFrameResource.screenTexelSize = DirectX::XMFLOAT2(1.f / Graphics::getWidth(), 1.f / Graphics::getHeight());

		GraphicsContext::globalConstantBuffer->update(&perFrameResource, sizeof(PerFrameResource));
	}

	if (Graphics::getFrameIndex() == 0)
	{
		GlobalDescriptorHeap::getResourceHeap()->resetDynamicDescriptorPointer();
	}
}

void RenderEngine::end()
{
	{
		CD3DX12_RESOURCE_BARRIER barriers[] =
		{
			CD3DX12_RESOURCE_BARRIER::Transition(backBufferResources[Graphics::getFrameIndex()].Get(),D3D12_RESOURCE_STATE_PRESENT,D3D12_RESOURCE_STATE_RENDER_TARGET),
			CD3DX12_RESOURCE_BARRIER::Transition(ConstantBuffer::bufferPools[0]->buffer->getResource(),D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER,D3D12_RESOURCE_STATE_COPY_DEST),
			CD3DX12_RESOURCE_BARRIER::Transition(ConstantBuffer::bufferPools[1]->buffer->getResource(),D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER,D3D12_RESOURCE_STATE_COPY_DEST),
			CD3DX12_RESOURCE_BARRIER::Transition(ConstantBuffer::bufferPools[2]->buffer->getResource(),D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER,D3D12_RESOURCE_STATE_COPY_DEST)
		};

		beginCommandlist->get()->ResourceBarrier(_countof(barriers), barriers);

		for (UINT bufferPoolIndex = 0; bufferPoolIndex < Graphics::FrameBufferCount; bufferPoolIndex++)
		{
			ConstantBuffer::bufferPools[bufferPoolIndex]->recordCommands(beginCommandlist->get());
		}

		{
			CD3DX12_RESOURCE_BARRIER barriers[] =
			{
				CD3DX12_RESOURCE_BARRIER::Transition(ConstantBuffer::bufferPools[0]->buffer->getResource(),D3D12_RESOURCE_STATE_COPY_DEST,D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER),
				CD3DX12_RESOURCE_BARRIER::Transition(ConstantBuffer::bufferPools[1]->buffer->getResource(),D3D12_RESOURCE_STATE_COPY_DEST,D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER),
				CD3DX12_RESOURCE_BARRIER::Transition(ConstantBuffer::bufferPools[2]->buffer->getResource(),D3D12_RESOURCE_STATE_COPY_DEST,D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER)
			};

			beginCommandlist->get()->ResourceBarrier(_countof(barriers), barriers);
		}

		beginCommandlist->get()->Close();
	}

	{
		endCommandList->reset();

		CD3DX12_RESOURCE_BARRIER barriers[] =
		{
			CD3DX12_RESOURCE_BARRIER::Transition(backBufferResources[Graphics::getFrameIndex()].Get(),D3D12_RESOURCE_STATE_RENDER_TARGET,D3D12_RESOURCE_STATE_PRESENT),
		};

		endCommandList->get()->ResourceBarrier(_countof(barriers), barriers);

		endCommandList->get()->Close();

		commandLists.push_back(endCommandList->get());
	}

	processCommandLists();
}

GPUVendor RenderEngine::getVendor() const
{
	return vendor;
}

RenderEngine::RenderEngine(const HWND hwnd) :
	fenceValues{}, fenceEvent(nullptr), vendor(GPUVendor::UNKNOWN), perFrameResource{}
{
	ComPtr<ID3D12Debug> debugController;

	D3D12GetDebugInterface(IID_PPV_ARGS(&debugController));

	debugController->EnableDebugLayer();

	ComPtr<IDXGIFactory7> factory;

	CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, IID_PPV_ARGS(&factory));

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

	{
		GraphicsDevice::instance = new GraphicsDevice(adapter.Get());

		D3D12_COMMAND_QUEUE_DESC queueDesc = {};
		queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

		GraphicsDevice::get()->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&commandQueue));
	}

	States::initialize();

	GlobalDescriptorHeap::instance = new GlobalDescriptorHeap();

	GlobalRootSignature::instance = new GlobalRootSignature();

	for (UINT i = 0; i < Graphics::FrameBufferCount; i++)
	{
		ConstantBuffer::bufferPools[i] = new ConstantBufferPool(256 << i, 1024);
	}

	Shader::createGlobalShaders();

	{
		Graphics::backBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;

		DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
		swapChainDesc.BufferCount = Graphics::FrameBufferCount;
		swapChainDesc.Width = Graphics::getWidth();
		swapChainDesc.Height = Graphics::getHeight();
		swapChainDesc.Format = Graphics::getBackBufferFormat();
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

	beginCommandlist = new CommandList(D3D12_COMMAND_LIST_TYPE_DIRECT);
	endCommandList = new CommandList(D3D12_COMMAND_LIST_TYPE_DIRECT);

	{
		beginCommandlist->reset();

		CD3DX12_RESOURCE_BARRIER barriers[] =
		{
			CD3DX12_RESOURCE_BARRIER::Transition(ConstantBuffer::bufferPools[0]->buffer->getResource(),D3D12_RESOURCE_STATE_COPY_DEST,D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER),
			CD3DX12_RESOURCE_BARRIER::Transition(ConstantBuffer::bufferPools[1]->buffer->getResource(),D3D12_RESOURCE_STATE_COPY_DEST,D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER),
			CD3DX12_RESOURCE_BARRIER::Transition(ConstantBuffer::bufferPools[2]->buffer->getResource(),D3D12_RESOURCE_STATE_COPY_DEST,D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER)
		};

		beginCommandlist->get()->ResourceBarrier(_countof(barriers), barriers);

		beginCommandlist->get()->Close();

		commandLists.push_back(beginCommandlist->get());

		GraphicsContext::globalConstantBuffer = new ConstantBuffer(sizeof(PerFrameResource), true, nullptr, nullptr, nullptr);
	}

	{
		DescriptorHandle descriptorHandle = GlobalDescriptorHeap::getRenderTargetHeap()->allocStaticDescriptor(Graphics::FrameBufferCount);

		for (UINT i = 0; i < Graphics::FrameBufferCount; i++)
		{
			swapChain->GetBuffer(i, IID_PPV_ARGS(&backBufferResources[i]));

			GraphicsDevice::get()->CreateRenderTargetView(backBufferResources[i].Get(), nullptr, descriptorHandle.getCPUHandle());

			Graphics::backBufferHandles[i] = descriptorHandle.getCPUHandle();

			descriptorHandle.move();
		}
	}
}

RenderEngine::~RenderEngine()
{
	if (GlobalDescriptorHeap::instance)
	{
		delete GlobalDescriptorHeap::instance;
	}

	if (GlobalRootSignature::instance)
	{
		delete GlobalRootSignature::instance;
	}

	if (GraphicsContext::globalConstantBuffer)
	{
		delete GraphicsContext::globalConstantBuffer;
	}

	Shader::releaseGlobalShaders();

	if (beginCommandlist)
	{
		delete beginCommandlist;
	}

	if (endCommandList)
	{
		delete endCommandList;
	}

	for (UINT i = 0; i < Graphics::FrameBufferCount; i++)
	{
		if (ConstantBuffer::bufferPools[i])
		{
			delete ConstantBuffer::bufferPools[i];
		}
	}
}
