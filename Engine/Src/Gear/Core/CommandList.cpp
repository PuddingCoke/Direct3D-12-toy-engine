#include<Gear/Core/CommandList.h>

CommandList::CommandList(const D3D12_COMMAND_LIST_TYPE type)
{
	for (UINT i = 0; i < Graphics::FrameBufferCount; i++)
	{
		allocators[i] = new CommandAllocator(type);
	}

	GraphicsDevice::get()->CreateCommandList(0, type, allocators[Graphics::getFrameIndex()]->get(), nullptr, IID_PPV_ARGS(&commandList));

	commandList->Close();
}

CommandList::~CommandList()
{
	for (UINT i = 0; i < Graphics::FrameBufferCount; i++)
	{
		if (allocators[i])
		{
			delete allocators[i];
		}
	}
}

void CommandList::reset() const
{
	allocators[Graphics::getFrameIndex()]->get()->Reset();

	commandList->Reset(allocators[Graphics::getFrameIndex()]->get(), nullptr);
}

void CommandList::setDescriptorHeap(DescriptorHeap* const resourceHeap, DescriptorHeap* const samplerHeap) const
{
	ID3D12DescriptorHeap* descriptorHeaps[2] = { resourceHeap->get(),samplerHeap->get() };

	commandList->SetDescriptorHeaps(2, descriptorHeaps);
}

void CommandList::setGraphicsRootSignature(RootSignature* const rootSignature) const
{
	commandList->SetGraphicsRootSignature(rootSignature->get());
}

void CommandList::setComputeRootSignature(RootSignature* const rootSignature) const
{
	commandList->SetComputeRootSignature(rootSignature->get());
}

ID3D12GraphicsCommandList6* CommandList::get() const
{
	return commandList.Get();
}

void CommandList::transitionResources()
{
	for (Buffer* const buff : transitionBuffers)
	{
		buff->transition(transitionBarriers, pendingBufferBarrier);
	}

	transitionBuffers.clear();

	for (Texture* const tex : transitionTextures)
	{
		tex->transition(transitionBarriers, pendingTextureBarrier);
	}

	transitionTextures.clear();

	if (transitionBarriers.size())
	{
		commandList->ResourceBarrier(transitionBarriers.size(), transitionBarriers.data());

		transitionBarriers.clear();
	}
}

void CommandList::updateReferredResStates()
{
	for (Resource* const res : referredResources)
	{
		res->updateGlobalStates();

		if (res->isSharedResource())
		{
			res->resetInternalStates();
		}
	}

	referredResources.clear();
}

void CommandList::pushResourceTrackList(Texture* const texture)
{
	if (texture->getStateTracking())
	{
		if (referredResources.find(texture) == referredResources.end())
		{
			referredResources.insert(texture);
		}

		if (transitionTextures.find(texture) == transitionTextures.end())
		{
			transitionTextures.insert(texture);
		}
	}
}

void CommandList::pushResourceTrackList(Buffer* const buffer)
{
	if (buffer->getStateTracking())
	{
		if (referredResources.find(buffer) == referredResources.end())
		{
			referredResources.insert(buffer);
		}

		if (transitionBuffers.find(buffer) == transitionBuffers.end())
		{
			transitionBuffers.insert(buffer);
		}
	}
}

void CommandList::setAllPipelineResources(const std::vector<ShaderResourceDesc>& descs)
{
	setAllResources(descs);
}

void CommandList::setAllPipelineResources(const std::initializer_list<ShaderResourceDesc>& descs)
{
	setAllResources(descs);
}

void CommandList::setGraphicsPipelineResources(const std::vector<ShaderResourceDesc>& descs, const UINT targetSRVState)
{
	setGraphicsResources(descs, targetSRVState);
}

void CommandList::setGraphicsPipelineResources(const std::initializer_list<ShaderResourceDesc>& descs, const UINT targetSRVState)
{
	setGraphicsResources(descs, targetSRVState);
}

void CommandList::setComputePipelineResources(const std::vector<ShaderResourceDesc>& descs)
{
	setComputeResources(descs);
}

void CommandList::setComputePipelineResources(const std::initializer_list<ShaderResourceDesc>& descs)
{
	setComputeResources(descs);
}

void CommandList::setRenderTargets(const std::initializer_list<RenderTargetDesc>& renderTargets, const std::initializer_list<DepthStencilDesc>& depthStencils)
{
	std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> rtvHandles;

	std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> dsvHandles;

	for (const RenderTargetDesc& desc : renderTargets)
	{
		rtvHandles.push_back(desc.rtvHandle);

		Texture* const texture = desc.texture;

		pushResourceTrackList(texture);

		texture->setMipSliceState(desc.mipSlice, D3D12_RESOURCE_STATE_RENDER_TARGET);
	}

	//not support read only dsv currently
	for (const DepthStencilDesc& desc : depthStencils)
	{
		dsvHandles.push_back(desc.dsvHandle);

		Texture* const texture = desc.texture;

		pushResourceTrackList(texture);

		texture->setMipSliceState(desc.mipSlice, D3D12_RESOURCE_STATE_DEPTH_WRITE);
	}

	if (dsvHandles.size() > 0)
	{
		commandList->OMSetRenderTargets(rtvHandles.size(), rtvHandles.data(), FALSE, dsvHandles.data());
	}
	else
	{
		commandList->OMSetRenderTargets(rtvHandles.size(), rtvHandles.data(), FALSE, nullptr);
	}
}

void CommandList::uavBarrier(const std::initializer_list<Resource*>& resources)
{
	std::vector<D3D12_RESOURCE_BARRIER> barriers;

	for (const Resource* const resource : resources)
	{
		D3D12_RESOURCE_BARRIER barrier = {};
		barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_UAV;
		barrier.UAV.pResource = resource->getResource();

		barriers.push_back(barrier);
	}

	commandList->ResourceBarrier(barriers.size(), barriers.data());
}
