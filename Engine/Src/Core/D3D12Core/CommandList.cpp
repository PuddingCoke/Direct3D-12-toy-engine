#include<Gear/Core/D3D12Core/CommandList.h>

#include<Gear/Core/Graphics.h>

Gear::Core::D3D12Core::CommandList::CommandList(const D3D12_COMMAND_LIST_TYPE type)
{
	allocators = new ComPtr<ID3D12CommandAllocator>[Graphics::getFrameBufferCount()];

	for (uint32_t i = 0; i < Graphics::getFrameBufferCount(); i++)
	{
		GraphicsDevice::get()->CreateCommandAllocator(type, IID_PPV_ARGS(&allocators[i]));
	}

	GraphicsDevice::get()->CreateCommandList(0, type, allocators[Graphics::getFrameIndex()].Get(), nullptr, IID_PPV_ARGS(&commandList));

	commandList->Close();
}

Gear::Core::D3D12Core::CommandList::~CommandList()
{
	if (allocators)
	{
		delete[] allocators;
	}
}

void Gear::Core::D3D12Core::CommandList::resourceBarrier(const uint32_t numBarriers, const D3D12_RESOURCE_BARRIER* const pBarriers) const
{
	commandList->ResourceBarrier(numBarriers, pBarriers);
}

ID3D12GraphicsCommandList6* Gear::Core::D3D12Core::CommandList::get() const
{
	return commandList.Get();
}

void Gear::Core::D3D12Core::CommandList::open() const
{
	allocators[Graphics::getFrameIndex()].Get()->Reset();

	commandList->Reset(allocators[Graphics::getFrameIndex()].Get(), nullptr);
}

void Gear::Core::D3D12Core::CommandList::close() const
{
	commandList->Close();
}

void Gear::Core::D3D12Core::CommandList::setDescriptorHeap(DescriptorHeap* const resourceHeap, DescriptorHeap* const samplerHeap) const
{
	ID3D12DescriptorHeap* descriptorHeaps[2] = { resourceHeap->get(),samplerHeap->get() };

	commandList->SetDescriptorHeaps(2, descriptorHeaps);
}

void Gear::Core::D3D12Core::CommandList::setGraphicsRootSignature(RootSignature* const rootSignature) const
{
	commandList->SetGraphicsRootSignature(rootSignature->get());
}

void Gear::Core::D3D12Core::CommandList::setComputeRootSignature(RootSignature* const rootSignature) const
{
	commandList->SetComputeRootSignature(rootSignature->get());
}

void Gear::Core::D3D12Core::CommandList::setGraphicsRootConstantBuffer(const uint32_t rootParameterIndex, const D3D12_GPU_VIRTUAL_ADDRESS bufferLocation) const
{
	commandList->SetGraphicsRootConstantBufferView(rootParameterIndex, bufferLocation);
}

void Gear::Core::D3D12Core::CommandList::setComputeRootConstantBuffer(const uint32_t rootParameterIndex, const D3D12_GPU_VIRTUAL_ADDRESS bufferLocation) const
{
	commandList->SetComputeRootConstantBufferView(rootParameterIndex, bufferLocation);
}

void Gear::Core::D3D12Core::CommandList::setGraphicsRootConstants(const uint32_t rootParameterIndex, const uint32_t num32BitValuesToSet, const void* const pSrcData, const uint32_t destOffsetIn32BitValues) const
{
	commandList->SetGraphicsRoot32BitConstants(rootParameterIndex, num32BitValuesToSet, pSrcData, destOffsetIn32BitValues);
}

void Gear::Core::D3D12Core::CommandList::setComputeRootConstants(const uint32_t rootParameterIndex, const uint32_t num32BitValuesToSet, const void* const pSrcData, const uint32_t destOffsetIn32BitValues) const
{
	commandList->SetComputeRoot32BitConstants(rootParameterIndex, num32BitValuesToSet, pSrcData, destOffsetIn32BitValues);
}

void Gear::Core::D3D12Core::CommandList::drawInstanced(const uint32_t vertexCountPerInstance, const uint32_t instanceCount, const uint32_t startVertexLocation, const uint32_t startInstanceLocation) const
{
	commandList->DrawInstanced(vertexCountPerInstance, instanceCount, startVertexLocation, startInstanceLocation);
}

void Gear::Core::D3D12Core::CommandList::drawIndexedInstanced(const uint32_t indexCountPerInstance, const uint32_t instanceCount, const uint32_t startIndexLocation, const int32_t baseVertexLocation, const uint32_t startInstanceLocation) const
{
	commandList->DrawIndexedInstanced(indexCountPerInstance, instanceCount, startIndexLocation, baseVertexLocation, startInstanceLocation);
}

void Gear::Core::D3D12Core::CommandList::dispatch(const uint32_t threadGroupCountX, const uint32_t threadGroupCountY, const uint32_t threadGroupCountZ) const
{
	commandList->Dispatch(threadGroupCountX, threadGroupCountY, threadGroupCountZ);
}

void Gear::Core::D3D12Core::CommandList::setPrimitiveTopology(const D3D12_PRIMITIVE_TOPOLOGY primitiveTopology) const
{
	commandList->IASetPrimitiveTopology(primitiveTopology);
}

void Gear::Core::D3D12Core::CommandList::setViewports(const uint32_t numViewports, const D3D12_VIEWPORT* const pViewports) const
{
	commandList->RSSetViewports(numViewports, pViewports);
}

void Gear::Core::D3D12Core::CommandList::setScissorRects(const uint32_t numRects, const D3D12_RECT* const pRects) const
{
	commandList->RSSetScissorRects(numRects, pRects);
}

void Gear::Core::D3D12Core::CommandList::setPipelineState(ID3D12PipelineState* const pipelineState) const
{
	commandList->SetPipelineState(pipelineState);
}

void Gear::Core::D3D12Core::CommandList::clearRenderTargetView(const D3D12_CPU_DESCRIPTOR_HANDLE renderTargetView, const float colorRGBA[4], const uint32_t numRects, const D3D12_RECT* const pRects) const
{
	commandList->ClearRenderTargetView(renderTargetView, colorRGBA, numRects, pRects);
}

void Gear::Core::D3D12Core::CommandList::clearDepthStencilView(const D3D12_CPU_DESCRIPTOR_HANDLE depthStencilView, const D3D12_CLEAR_FLAGS clearFlags, const float depth, const uint8_t stencil, const uint32_t numRects, const D3D12_RECT* const pRects) const
{
	commandList->ClearDepthStencilView(depthStencilView, clearFlags, depth, stencil, numRects, pRects);
}

void Gear::Core::D3D12Core::CommandList::transitionResources()
{
	for (Resource::D3D12Resource::Buffer* const buff : transitionBuffers)
	{
		buff->transition(transitionBarriers, pendingBufferBarrier);

		buff->popFromTrackingList();
	}

	transitionBuffers.clear();

	for (Resource::D3D12Resource::Texture* const tex : transitionTextures)
	{
		tex->transition(transitionBarriers, pendingTextureBarrier);

		tex->popFromTrackingList();
	}

	transitionTextures.clear();

	if (transitionBarriers.size())
	{
		commandList->ResourceBarrier(static_cast<uint32_t>(transitionBarriers.size()), transitionBarriers.data());

		transitionBarriers.clear();
	}
}

void Gear::Core::D3D12Core::CommandList::updateReferredSharedResourceStates()
{
	for (Resource::D3D12Resource::D3D12ResourceBase* const res : referredResources)
	{
		res->updateGlobalStates();

		res->resetInternalStates();

		res->popFromReferredList();
	}

	referredResources.clear();
}

void Gear::Core::D3D12Core::CommandList::pushResourceTrackList(Resource::D3D12Resource::Texture* const texture)
{
	texture->pushToReferredList(referredResources);

	texture->pushToTrackingList(transitionTextures);
}

void Gear::Core::D3D12Core::CommandList::pushResourceTrackList(Resource::D3D12Resource::Buffer* const buffer)
{
	buffer->pushToReferredList(referredResources);

	buffer->pushToTrackingList(transitionBuffers);
}

void Gear::Core::D3D12Core::CommandList::setAllPipelineResources(const std::vector<Resource::D3D12Resource::ShaderResourceDesc>& descs)
{
	setPipelineResources(descs, D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE);
}

void Gear::Core::D3D12Core::CommandList::setAllPipelineResources(const std::initializer_list<Resource::D3D12Resource::ShaderResourceDesc>& descs)
{
	setPipelineResources(descs, D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE);
}

void Gear::Core::D3D12Core::CommandList::setGraphicsPipelineResources(const std::vector<Resource::D3D12Resource::ShaderResourceDesc>& descs, const uint32_t targetSRVState)
{
	setPipelineResources(descs, targetSRVState);
}

void Gear::Core::D3D12Core::CommandList::setGraphicsPipelineResources(const std::initializer_list<Resource::D3D12Resource::ShaderResourceDesc>& descs, const uint32_t targetSRVState)
{
	setPipelineResources(descs, targetSRVState);
}

void Gear::Core::D3D12Core::CommandList::setComputePipelineResources(const std::vector<Resource::D3D12Resource::ShaderResourceDesc>& descs)
{
	setPipelineResources(descs, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
}

void Gear::Core::D3D12Core::CommandList::setComputePipelineResources(const std::initializer_list<Resource::D3D12Resource::ShaderResourceDesc>& descs)
{
	setPipelineResources(descs, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
}

void Gear::Core::D3D12Core::CommandList::setDefRenderTarget() const
{
	const D3D12_CPU_DESCRIPTOR_HANDLE backBufferHandle = Graphics::getBackBufferHandle();

	commandList->OMSetRenderTargets(1, &backBufferHandle, FALSE, nullptr);
}

void Gear::Core::D3D12Core::CommandList::clearDefRenderTarget(const float clearValue[4]) const
{
	commandList->ClearRenderTargetView(Graphics::getBackBufferHandle(), clearValue, 0, nullptr);
}

void Gear::Core::D3D12Core::CommandList::setRenderTargets(const std::initializer_list<Resource::D3D12Resource::RenderTargetDesc>& renderTargets, const Resource::D3D12Resource::DepthStencilDesc* const depthStencils)
{
	transientRTVHandles.clear();

	for (const Resource::D3D12Resource::RenderTargetDesc& desc : renderTargets)
	{
		transientRTVHandles.emplace_back(desc.rtvHandle);

		Resource::D3D12Resource::Texture* const texture = desc.texture;

		pushResourceTrackList(texture);

		texture->setMipSliceState(desc.mipSlice, D3D12_RESOURCE_STATE_RENDER_TARGET);
	}

	//目前不支持只读DSV
	if (depthStencils)
	{
		Resource::D3D12Resource::Texture* const texture = depthStencils->texture;

		pushResourceTrackList(texture);

		texture->setMipSliceState(depthStencils->mipSlice, D3D12_RESOURCE_STATE_DEPTH_WRITE);
	}

	if (depthStencils)
	{
		commandList->OMSetRenderTargets(static_cast<uint32_t>(transientRTVHandles.size()), transientRTVHandles.data(), FALSE, &(depthStencils->dsvHandle));
	}
	else
	{
		commandList->OMSetRenderTargets(static_cast<uint32_t>(transientRTVHandles.size()), transientRTVHandles.data(), FALSE, nullptr);
	}
}

void Gear::Core::D3D12Core::CommandList::setVertexBuffers(const uint32_t startSlot, const std::initializer_list<Resource::D3D12Resource::VertexBufferDesc>& vertexBuffers)
{
	transientVBViews.clear();

	for (const Resource::D3D12Resource::VertexBufferDesc& desc : vertexBuffers)
	{
		transientVBViews.emplace_back(desc.vbv);

		Resource::D3D12Resource::Buffer* const buffer = desc.buffer;

		pushResourceTrackList(buffer);

		buffer->setState(D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
	}

	commandList->IASetVertexBuffers(startSlot, static_cast<uint32_t>(transientVBViews.size()), transientVBViews.data());
}

void Gear::Core::D3D12Core::CommandList::setIndexBuffer(const Resource::D3D12Resource::IndexBufferDesc& indexBuffer)
{
	Resource::D3D12Resource::Buffer* const buffer = indexBuffer.buffer;

	pushResourceTrackList(buffer);

	buffer->setState(D3D12_RESOURCE_STATE_INDEX_BUFFER);

	commandList->IASetIndexBuffer(&indexBuffer.ibv);
}

void Gear::Core::D3D12Core::CommandList::copyBufferRegion(Resource::D3D12Resource::Buffer* const dstBuffer, const uint64_t dstOffset, Resource::D3D12Resource::UploadHeap* srcBuffer, const uint64_t srcOffset, const uint64_t numBytes)
{
	pushResourceTrackList(dstBuffer);

	dstBuffer->setState(D3D12_RESOURCE_STATE_COPY_DEST);

	transitionResources();

	commandList->CopyBufferRegion(dstBuffer->getResource(), dstOffset, srcBuffer->getResource(), srcOffset, numBytes);
}

void Gear::Core::D3D12Core::CommandList::copyBufferRegion(Resource::D3D12Resource::Buffer* const dstBuffer, const uint64_t dstOffset, Resource::D3D12Resource::Buffer* srcBuffer, const uint64_t srcOffset, const uint64_t numBytes)
{
	pushResourceTrackList(dstBuffer);

	dstBuffer->setState(D3D12_RESOURCE_STATE_COPY_DEST);

	pushResourceTrackList(srcBuffer);

	srcBuffer->setState(D3D12_RESOURCE_STATE_COPY_SOURCE);

	transitionResources();

	commandList->CopyBufferRegion(dstBuffer->getResource(), dstOffset, srcBuffer->getResource(), srcOffset, numBytes);
}

void Gear::Core::D3D12Core::CommandList::copyResource(Resource::D3D12Resource::Buffer* const dstBuffer, Resource::D3D12Resource::UploadHeap* const srcBuffer)
{
	pushResourceTrackList(dstBuffer);

	dstBuffer->setState(D3D12_RESOURCE_STATE_COPY_DEST);

	transitionResources();

	commandList->CopyResource(dstBuffer->getResource(), srcBuffer->getResource());
}

void Gear::Core::D3D12Core::CommandList::copyResource(Resource::D3D12Resource::Buffer* const dstBuffer, Resource::D3D12Resource::Buffer* const srcBuffer)
{
	pushResourceTrackList(dstBuffer);

	dstBuffer->setState(D3D12_RESOURCE_STATE_COPY_DEST);

	pushResourceTrackList(srcBuffer);

	srcBuffer->setState(D3D12_RESOURCE_STATE_COPY_SOURCE);

	transitionResources();

	commandList->CopyResource(dstBuffer->getResource(), srcBuffer->getResource());
}

void Gear::Core::D3D12Core::CommandList::copyTextureRegion(Resource::D3D12Resource::Texture* const dstTexture, const uint32_t dstSubresource, Resource::D3D12Resource::Texture* const srcTexture, const uint32_t srcSubresource)
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

void Gear::Core::D3D12Core::CommandList::uavBarrier(const std::initializer_list<Resource::D3D12Resource::D3D12ResourceBase*>& resources)
{
	transientUAVBarriers.clear();

	for (const Resource::D3D12Resource::D3D12ResourceBase* const resource : resources)
	{
		D3D12_RESOURCE_BARRIER barrier = {};
		barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_UAV;
		barrier.UAV.pResource = resource->getResource();

		transientUAVBarriers.emplace_back(barrier);
	}

	commandList->ResourceBarrier(static_cast<uint32_t>(transientUAVBarriers.size()), transientUAVBarriers.data());
}

void Gear::Core::D3D12Core::CommandList::clearUnorderedAccessView(const Resource::D3D12Resource::ClearUAVDesc& desc, const float values[4])
{
	ID3D12Resource* resource = nullptr;

	if (desc.type == Resource::D3D12Resource::ShaderResourceDesc::BUFFER)
	{
		Resource::D3D12Resource::Buffer* const buffer = desc.bufferDesc.buffer;

		resource = buffer->getResource();

		pushResourceTrackList(buffer);

		buffer->setState(D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	}
	else if (desc.type == Resource::D3D12Resource::ShaderResourceDesc::TEXTURE)
	{
		Resource::D3D12Resource::Texture* const texture = desc.textureDesc.texture;

		resource = texture->getResource();

		pushResourceTrackList(texture);

		texture->setMipSliceState(desc.textureDesc.mipSlice, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	}

	transitionResources();

	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_UAV;
	barrier.UAV.pResource = resource;

	commandList->ResourceBarrier(1, &barrier);

	commandList->ClearUnorderedAccessViewFloat(desc.viewGPUHandle, desc.viewCPUHandle, resource, values, 0, nullptr);

	commandList->ResourceBarrier(1, &barrier);
}

void Gear::Core::D3D12Core::CommandList::clearUnorderedAccessView(const Resource::D3D12Resource::ClearUAVDesc& desc, const uint32_t values[4])
{
	ID3D12Resource* resource = nullptr;

	if (desc.type == Resource::D3D12Resource::ShaderResourceDesc::BUFFER)
	{
		Resource::D3D12Resource::Buffer* const buffer = desc.bufferDesc.buffer;

		resource = buffer->getResource();

		pushResourceTrackList(buffer);

		buffer->setState(D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	}
	else if (desc.type == Resource::D3D12Resource::ShaderResourceDesc::TEXTURE)
	{
		Resource::D3D12Resource::Texture* const texture = desc.textureDesc.texture;

		resource = texture->getResource();

		pushResourceTrackList(texture);

		texture->setMipSliceState(desc.textureDesc.mipSlice, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	}

	transitionResources();

	commandList->ClearUnorderedAccessViewUint(desc.viewGPUHandle, desc.viewCPUHandle, resource, values, 0, nullptr);
}

void Gear::Core::D3D12Core::CommandList::solvePendingBarriers(std::vector<D3D12_RESOURCE_BARRIER>& barriers)
{
	if (pendingBufferBarrier.size())
	{
		for (const Resource::D3D12Resource::PendingBufferBarrier& pendingBarrier : pendingBufferBarrier)
		{
			pendingBarrier.buffer->solvePendingBarrier(barriers, pendingBarrier.afterState);
		}

		pendingBufferBarrier.clear();
	}

	if (pendingTextureBarrier.size())
	{
		for (const Resource::D3D12Resource::PendingTextureBarrier& pendingBarrier : pendingTextureBarrier)
		{
			pendingBarrier.texture->solvePendingBarrier(barriers, pendingBarrier.mipSlice, pendingBarrier.afterState);
		}

		pendingTextureBarrier.clear();
	}
}
