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

void CommandList::close() const
{
	commandList->Close();
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
	setPipelineResources(descs, D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE);
}

void CommandList::setAllPipelineResources(const std::initializer_list<ShaderResourceDesc>& descs)
{
	setPipelineResources(descs, D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE);
}

void CommandList::setGraphicsPipelineResources(const std::vector<ShaderResourceDesc>& descs, const UINT targetSRVState)
{
	setPipelineResources(descs, targetSRVState);
}

void CommandList::setGraphicsPipelineResources(const std::initializer_list<ShaderResourceDesc>& descs, const UINT targetSRVState)
{
	setPipelineResources(descs, targetSRVState);
}

void CommandList::setComputePipelineResources(const std::vector<ShaderResourceDesc>& descs)
{
	setPipelineResources(descs, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
}

void CommandList::setComputePipelineResources(const std::initializer_list<ShaderResourceDesc>& descs)
{
	setPipelineResources(descs, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
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

void CommandList::setVertexBuffers(const UINT startSlot, const std::initializer_list<VertexBufferDesc>& vertexBuffers)
{
	std::vector<D3D12_VERTEX_BUFFER_VIEW> vbvs;

	for (const VertexBufferDesc& desc : vertexBuffers)
	{
		vbvs.push_back(desc.vbv);

		Buffer* const buffer = desc.buffer;

		pushResourceTrackList(buffer);

		buffer->setState(D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
	}

	commandList->IASetVertexBuffers(startSlot, vbvs.size(), vbvs.data());
}

void CommandList::setIndexBuffer(const IndexBufferDesc indexBuffer)
{
	Buffer* const buffer = indexBuffer.buffer;
	
	pushResourceTrackList(buffer);

	buffer->setState(D3D12_RESOURCE_STATE_INDEX_BUFFER);

	commandList->IASetIndexBuffer(&indexBuffer.ibv);
}

void CommandList::copyBufferRegion(Buffer* const dstBuffer, const UINT64 dstOffset, UploadHeap* srcBuffer, const UINT64 srcOffset, const UINT64 numBytes)
{
	pushResourceTrackList(dstBuffer);

	dstBuffer->setState(D3D12_RESOURCE_STATE_COPY_DEST);

	transitionResources();

	commandList->CopyBufferRegion(dstBuffer->getResource(), dstOffset, srcBuffer->getResource(), srcOffset, numBytes);
}

void CommandList::copyBufferRegion(Buffer* const dstBuffer, const UINT64 dstOffset, Buffer* srcBuffer, const UINT64 srcOffset, const UINT64 numBytes)
{
	pushResourceTrackList(dstBuffer);

	dstBuffer->setState(D3D12_RESOURCE_STATE_COPY_DEST);

	pushResourceTrackList(srcBuffer);

	srcBuffer->setState(D3D12_RESOURCE_STATE_COPY_SOURCE);

	transitionResources();

	commandList->CopyBufferRegion(dstBuffer->getResource(), dstOffset, srcBuffer->getResource(), srcOffset, numBytes);
}

void CommandList::copyResource(Buffer* const dstBuffer, UploadHeap* const srcBuffer)
{
	pushResourceTrackList(dstBuffer);

	dstBuffer->setState(D3D12_RESOURCE_STATE_COPY_DEST);

	transitionResources();

	commandList->CopyResource(dstBuffer->getResource(), srcBuffer->getResource());
}

void CommandList::copyResource(Buffer* const dstBuffer, Buffer* const srcBuffer)
{
	pushResourceTrackList(dstBuffer);

	dstBuffer->setState(D3D12_RESOURCE_STATE_COPY_DEST);

	pushResourceTrackList(srcBuffer);

	srcBuffer->setState(D3D12_RESOURCE_STATE_COPY_SOURCE);

	transitionResources();

	commandList->CopyResource(dstBuffer->getResource(), srcBuffer->getResource());
}

void CommandList::copyTextureRegion(Texture* const dstTexture, const UINT dstSubresource, Texture* const srcTexture, const UINT srcSubresource)
{
	pushResourceTrackList(dstTexture);

	dstTexture->setAllState(D3D12_RESOURCE_STATE_COPY_DEST);

	pushResourceTrackList(srcTexture);

	srcTexture->setAllState(D3D12_RESOURCE_STATE_COPY_SOURCE);

	transitionResources();

	D3D12_TEXTURE_COPY_LOCATION dstLocation = {};
	dstLocation.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
	dstLocation.pResource = dstTexture->getResource();
	dstLocation.SubresourceIndex = dstSubresource;

	D3D12_TEXTURE_COPY_LOCATION srcLocation = {};
	srcLocation.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
	srcLocation.pResource = srcTexture->getResource();
	srcLocation.SubresourceIndex = srcSubresource;

	commandList->CopyTextureRegion(&dstLocation, 0, 0, 0, &srcLocation, nullptr);
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
