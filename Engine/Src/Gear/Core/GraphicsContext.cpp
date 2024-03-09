#include<Gear/Core/GraphicsContext.h>

ConstantBuffer* GraphicsContext::globalConstantBuffer = nullptr;

GraphicsContext::GraphicsContext():
	commandList(new CommandList(D3D12_COMMAND_LIST_TYPE_DIRECT)),
	vp{ 0.f,0.f,0.f,0.f,0.f,1.f },
	rt{ 0,0,0,0 }
{
}

GraphicsContext::~GraphicsContext()
{
	if (commandList)
	{
		delete commandList;
	}
}

void GraphicsContext::updateBuffer(VertexBuffer* const vb, const void* const data, const UINT size)
{
	Buffer* const buffer = vb->getBuffer();

	pushResourceTrackList(buffer);

	buffer->transitionState = D3D12_RESOURCE_STATE_COPY_DEST;

	flushTransitionResources();

	UploadHeap* const uploadHeap = vb->uploadHeaps[vb->uploadHeapIndex];

	uploadHeap->update(data, size);

	commandList->get()->CopyBufferRegion(buffer->getResource(), 0, uploadHeap->getResource(), 0, size);

	vb->uploadHeapIndex = (vb->uploadHeapIndex + 1) % Graphics::FrameBufferCount;
}

void GraphicsContext::updateBuffer(IndexBuffer* const ib, const void* const data, const UINT size)
{
	Buffer* const buffer = ib->getBuffer();

	pushResourceTrackList(buffer);

	buffer->transitionState = D3D12_RESOURCE_STATE_COPY_DEST;

	flushTransitionResources();

	UploadHeap* const uploadHeap = ib->uploadHeaps[ib->uploadHeapIndex];

	uploadHeap->update(data, size);

	commandList->get()->CopyBufferRegion(buffer->getResource(), 0, uploadHeap->getResource(), 0, size);

	ib->uploadHeapIndex = (ib->uploadHeapIndex + 1) % Graphics::FrameBufferCount;
}

void GraphicsContext::setGlobalIndexBuffer(const IndexConstantBuffer* const indexBuffer)
{
	for (const ShaderResourceDesc& desc : indexBuffer->descs)
	{
		if (desc.type == ShaderResourceDesc::BUFFER)
		{
			Buffer* const buffer = desc.bufferDesc.buffer;

			if (buffer->getStateTracking() && desc.state != ShaderResourceDesc::CBV)
			{
				pushResourceTrackList(buffer);

				if (desc.state == ShaderResourceDesc::SRV)
				{
					buffer->transitionState = D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE;
				}
				else if (desc.state == ShaderResourceDesc::UAV)
				{
					buffer->transitionState = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
				}
			}
		}
		else if (desc.type == ShaderResourceDesc::TEXTURE)
		{
			Texture* const texture = desc.textureDesc.texture;

			if (texture->getStateTracking())
			{
				pushResourceTrackList(texture);

				if (desc.state == ShaderResourceDesc::SRV)
				{
					if (desc.textureDesc.mipSlice == D3D12_TRANSITION_ALL_MIPLEVELS)
					{
						texture->transitionState.allState = D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE;

						for (UINT i = 0; i < texture->mipLevels; i++)
						{
							texture->transitionState.mipLevelStates[i] = D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE;
						}
					}
					else
					{
						if (texture->mipLevels > 1)
						{
							texture->transitionState.allState = D3D12_RESOURCE_STATE_UNKNOWN;

							texture->transitionState.mipLevelStates[desc.textureDesc.mipSlice] = D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE;
						}
						else
						{
							texture->transitionState.allState = D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE;

							texture->transitionState.mipLevelStates[desc.textureDesc.mipSlice] = D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE;
						}
					}
				}
				else if (desc.state == ShaderResourceDesc::UAV)
				{
					if (desc.textureDesc.mipSlice == D3D12_TRANSITION_ALL_MIPLEVELS)
					{
						texture->transitionState.allState = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;

						for (UINT i = 0; i < texture->mipLevels; i++)
						{
							texture->transitionState.mipLevelStates[i] = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
						}
					}
					else
					{
						if (texture->mipLevels > 1)
						{
							texture->transitionState.allState = D3D12_RESOURCE_STATE_UNKNOWN;

							texture->transitionState.mipLevelStates[desc.textureDesc.mipSlice] = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
						}
						else
						{
							texture->transitionState.allState = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;

							texture->transitionState.mipLevelStates[desc.textureDesc.mipSlice] = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
						}
					}
				}
			}
		}
	}

	flushTransitionResources();

	commandList->get()->SetGraphicsRootConstantBufferView(2, indexBuffer->getGPUAddress());
	commandList->get()->SetComputeRootConstantBufferView(2, indexBuffer->getGPUAddress());
}

void GraphicsContext::setComputeIndexBuffer(const IndexConstantBuffer* const indexBuffer)
{
	for (const ShaderResourceDesc& desc : indexBuffer->descs)
	{
		if (desc.type == ShaderResourceDesc::BUFFER)
		{
			Buffer* const buffer = desc.bufferDesc.buffer;

			if (buffer->getStateTracking() && desc.state != ShaderResourceDesc::CBV)
			{
				pushResourceTrackList(buffer);

				if (desc.state == ShaderResourceDesc::SRV)
				{
					buffer->transitionState = D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
				}
				else if (desc.state == ShaderResourceDesc::UAV)
				{
					buffer->transitionState = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
				}
			}
		}
		else if (desc.type == ShaderResourceDesc::TEXTURE)
		{
			Texture* const texture = desc.textureDesc.texture;

			if (texture->getStateTracking())
			{
				pushResourceTrackList(texture);

				if (desc.state == ShaderResourceDesc::SRV)
				{
					if (desc.textureDesc.mipSlice == D3D12_TRANSITION_ALL_MIPLEVELS)
					{
						texture->transitionState.allState = D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;

						for (UINT i = 0; i < texture->mipLevels; i++)
						{
							texture->transitionState.mipLevelStates[i] = D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
						}
					}
					else
					{
						if (texture->mipLevels > 1)
						{
							texture->transitionState.allState = D3D12_RESOURCE_STATE_UNKNOWN;

							texture->transitionState.mipLevelStates[desc.textureDesc.mipSlice] = D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
						}
						else
						{
							texture->transitionState.allState = D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;

							texture->transitionState.mipLevelStates[desc.textureDesc.mipSlice] = D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
						}
					}
				}
				else if (desc.state == ShaderResourceDesc::UAV)
				{
					if (desc.textureDesc.mipSlice == D3D12_TRANSITION_ALL_MIPLEVELS)
					{
						texture->transitionState.allState = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;

						for (UINT i = 0; i < texture->mipLevels; i++)
						{
							texture->transitionState.mipLevelStates[i] = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
						}
					}
					else
					{
						if (texture->mipLevels > 1)
						{
							texture->transitionState.allState = D3D12_RESOURCE_STATE_UNKNOWN;

							texture->transitionState.mipLevelStates[desc.textureDesc.mipSlice] = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
						}
						else
						{
							texture->transitionState.allState = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;

							texture->transitionState.mipLevelStates[desc.textureDesc.mipSlice] = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
						}
					}
				}
			}
		}
	}

	flushTransitionResources();

	commandList->get()->SetComputeRootConstantBufferView(3, indexBuffer->getGPUAddress());
}

void GraphicsContext::setGraphicsConstants(const std::initializer_list<ShaderResourceDesc> descs, const UINT offset)
{
	UINT index = 0;

	UINT indices[16] = {};

	for (const ShaderResourceDesc& desc : descs)
	{
		if (desc.type == ShaderResourceDesc::BUFFER)
		{
			indices[index] = desc.bufferDesc.resourceIndex;

			Buffer* const buffer = desc.bufferDesc.buffer;

			if (buffer->getStateTracking() && desc.state != ShaderResourceDesc::CBV)
			{
				pushResourceTrackList(buffer);

				if (desc.state == ShaderResourceDesc::SRV)
				{
					buffer->transitionState = D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE;
				}
				else if (desc.state == ShaderResourceDesc::UAV)
				{
					buffer->transitionState = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
				}
			}
		}
		else if (desc.type == ShaderResourceDesc::TEXTURE)
		{
			indices[index] = desc.textureDesc.resourceIndex;

			Texture* const texture = desc.textureDesc.texture;

			if (texture->getStateTracking())
			{
				pushResourceTrackList(texture);

				if (desc.state == ShaderResourceDesc::SRV)
				{
					if (desc.textureDesc.mipSlice == D3D12_TRANSITION_ALL_MIPLEVELS)
					{
						texture->transitionState.allState = D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE;

						for (UINT i = 0; i < texture->mipLevels; i++)
						{
							texture->transitionState.mipLevelStates[i] = D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE;
						}
					}
					else
					{
						if (texture->mipLevels > 1)
						{
							texture->transitionState.allState = D3D12_RESOURCE_STATE_UNKNOWN;

							texture->transitionState.mipLevelStates[desc.textureDesc.mipSlice] = D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE;
						}
						else
						{
							texture->transitionState.allState = D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE;

							texture->transitionState.mipLevelStates[desc.textureDesc.mipSlice] = D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE;
						}
					}
				}
				else if (desc.state == ShaderResourceDesc::UAV)
				{
					if (desc.textureDesc.mipSlice == D3D12_TRANSITION_ALL_MIPLEVELS)
					{
						texture->transitionState.allState = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;

						for (UINT i = 0; i < texture->mipLevels; i++)
						{
							texture->transitionState.mipLevelStates[i] = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
						}
					}
					else
					{
						if (texture->mipLevels > 1)
						{
							texture->transitionState.allState = D3D12_RESOURCE_STATE_UNKNOWN;

							texture->transitionState.mipLevelStates[desc.textureDesc.mipSlice] = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
						}
						else
						{
							texture->transitionState.allState = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;

							texture->transitionState.mipLevelStates[desc.textureDesc.mipSlice] = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
						}
					}
				}
			}
		}

		index++;
	}

	flushTransitionResources();

	setGraphicsConstants(descs.size(), indices, offset);
}

void GraphicsContext::setGraphicsConstants(const UINT numValues, const void* const data, const UINT offset) const
{
	commandList->get()->SetGraphicsRoot32BitConstants(1, numValues, data, offset);
}

void GraphicsContext::setComputeConstants(const std::initializer_list<ShaderResourceDesc> descs, const UINT offset)
{
	UINT index = 0;

	UINT indices[16] = {};

	for (const ShaderResourceDesc& desc : descs)
	{
		if (desc.type == ShaderResourceDesc::BUFFER)
		{
			indices[index] = desc.bufferDesc.resourceIndex;

			Buffer* const buffer = desc.bufferDesc.buffer;

			if (buffer->getStateTracking() && desc.state != ShaderResourceDesc::CBV)
			{
				pushResourceTrackList(buffer);

				if (desc.state == ShaderResourceDesc::SRV)
				{
					buffer->transitionState = D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
				}
				else if (desc.state == ShaderResourceDesc::UAV)
				{
					buffer->transitionState = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
				}
			}
		}
		else if (desc.type == ShaderResourceDesc::TEXTURE)
		{
			indices[index] = desc.textureDesc.resourceIndex;

			Texture* const texture = desc.textureDesc.texture;

			if (texture->getStateTracking())
			{
				pushResourceTrackList(texture);

				if (desc.state == ShaderResourceDesc::SRV)
				{
					if (desc.textureDesc.mipSlice == D3D12_TRANSITION_ALL_MIPLEVELS)
					{
						texture->transitionState.allState = D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;

						for (UINT i = 0; i < texture->mipLevels; i++)
						{
							texture->transitionState.mipLevelStates[i] = D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
						}
					}
					else
					{
						if (texture->mipLevels > 1)
						{
							texture->transitionState.allState = D3D12_RESOURCE_STATE_UNKNOWN;

							texture->transitionState.mipLevelStates[desc.textureDesc.mipSlice] = D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
						}
						else
						{
							texture->transitionState.allState = D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;

							texture->transitionState.mipLevelStates[desc.textureDesc.mipSlice] = D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
						}
					}
				}
				else if (desc.state == ShaderResourceDesc::UAV)
				{
					if (desc.textureDesc.mipSlice == D3D12_TRANSITION_ALL_MIPLEVELS)
					{
						texture->transitionState.allState = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;

						for (UINT i = 0; i < texture->mipLevels; i++)
						{
							texture->transitionState.mipLevelStates[i] = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
						}
					}
					else
					{
						if (texture->mipLevels > 1)
						{
							texture->transitionState.allState = D3D12_RESOURCE_STATE_UNKNOWN;

							texture->transitionState.mipLevelStates[desc.textureDesc.mipSlice] = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
						}
						else
						{
							texture->transitionState.allState = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;

							texture->transitionState.mipLevelStates[desc.textureDesc.mipSlice] = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
						}
					}
				}
			}
		}

		index++;
	}

	flushTransitionResources();

	setComputeConstants(descs.size(), indices, offset);
}

void GraphicsContext::setComputeConstants(const UINT numValues, const void* const data, const UINT offset) const
{
	commandList->get()->SetComputeRoot32BitConstants(1, numValues, data, offset);
}

void GraphicsContext::setVertexIndexBuffer(const IndexConstantBuffer* const indexBuffer)
{
	pushGraphicsStageIndexBuffer(9, indexBuffer, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
}

void GraphicsContext::setHullIndexBuffer(const IndexConstantBuffer* const indexBuffer)
{
	pushGraphicsStageIndexBuffer(6, indexBuffer, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
}

void GraphicsContext::setGeometryIndexBuffer(const IndexConstantBuffer* const indexBuffer)
{
	pushGraphicsStageIndexBuffer(5, indexBuffer, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
}

void GraphicsContext::setDomainIndexBuffer(const IndexConstantBuffer* const indexBuffer)
{
	pushGraphicsStageIndexBuffer(4, indexBuffer, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
}

void GraphicsContext::setPixelIndexBuffer(const IndexConstantBuffer* const indexBuffer)
{
	pushGraphicsStageIndexBuffer(8, indexBuffer, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
}

void GraphicsContext::setMeshIndexBuffer(const IndexConstantBuffer* const indexBuffer)
{
	pushGraphicsStageIndexBuffer(7, indexBuffer, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
}

void GraphicsContext::setAmplificationIndexBuffer(const IndexConstantBuffer* const indexBuffer)
{
	pushGraphicsStageIndexBuffer(3, indexBuffer, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
}

void GraphicsContext::finishGraphicsStageIndexBuffer()
{
	flushTransitionResources();
}

void GraphicsContext::setRenderTargets(const std::initializer_list<RenderTargetDesc>& renderTargets, const std::initializer_list<DepthStencilDesc>& depthStencils)
{
	std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> rtvHandles;

	std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> dsvHandles;

	for (const RenderTargetDesc& desc : renderTargets)
	{
		rtvHandles.push_back(desc.rtvHandle);

		Texture* const texture = desc.texture;

		pushResourceTrackList(texture);

		if (texture->mipLevels > 1)
		{
			texture->transitionState.allState = D3D12_RESOURCE_STATE_UNKNOWN;

			texture->transitionState.mipLevelStates[desc.mipSlice] = D3D12_RESOURCE_STATE_RENDER_TARGET;
		}
		else
		{
			texture->transitionState.allState = D3D12_RESOURCE_STATE_RENDER_TARGET;

			texture->transitionState.mipLevelStates[desc.mipSlice] = D3D12_RESOURCE_STATE_RENDER_TARGET;
		}
	}

	//not support read only dsv currently
	for (const DepthStencilDesc& desc : depthStencils)
	{
		dsvHandles.push_back(desc.dsvHandle);

		Texture* const texture = desc.texture;

		pushResourceTrackList(texture);

		if (texture->mipLevels > 1)
		{
			texture->transitionState.allState = D3D12_RESOURCE_STATE_UNKNOWN;

			texture->transitionState.mipLevelStates[desc.mipSlice] = D3D12_RESOURCE_STATE_DEPTH_WRITE;
		}
		else
		{
			texture->transitionState.allState = D3D12_RESOURCE_STATE_DEPTH_WRITE;

			texture->transitionState.mipLevelStates[desc.mipSlice] = D3D12_RESOURCE_STATE_DEPTH_WRITE;
		}
	}

	flushTransitionResources();

	if (dsvHandles.size() > 0)
	{
		commandList->get()->OMSetRenderTargets(rtvHandles.size(), rtvHandles.data(), FALSE, dsvHandles.data());
	}
	else
	{
		commandList->get()->OMSetRenderTargets(rtvHandles.size(), rtvHandles.data(), FALSE, nullptr);
	}
}

void GraphicsContext::setDefRenderTarget() const
{
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = Graphics::getBackBufferHandle();

	commandList->get()->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);
}

void GraphicsContext::clearDefRenderTarget(const FLOAT clearValue[4]) const
{
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = Graphics::getBackBufferHandle();

	commandList->get()->ClearRenderTargetView(rtvHandle, clearValue, 0, nullptr);
}

void GraphicsContext::setVertexBuffers(const UINT startSlot, const std::initializer_list<VertexBuffer*>& vertexBuffers)
{
	std::vector<D3D12_VERTEX_BUFFER_VIEW> vbvs;

	for (VertexBuffer* const vb : vertexBuffers)
	{
		Buffer* buffer = vb->getBuffer();

		pushResourceTrackList(buffer);

		buffer->transitionState = D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;

		vbvs.push_back(vb->getVertexBufferView());
	}

	flushTransitionResources();

	commandList->get()->IASetVertexBuffers(startSlot, vbvs.size(), vbvs.data());
}

void GraphicsContext::setIndexBuffers(const std::initializer_list<IndexBuffer*>& indexBuffers)
{
	std::vector<D3D12_INDEX_BUFFER_VIEW> ibvs;

	for (IndexBuffer* const ib : indexBuffers)
	{
		Buffer* buffer = ib->getBuffer();

		pushResourceTrackList(buffer);

		buffer->transitionState = D3D12_RESOURCE_STATE_INDEX_BUFFER;

		ibvs.push_back(ib->getIndexBufferView());
	}

	flushTransitionResources();

	commandList->get()->IASetIndexBuffer(ibvs.data());
}

void GraphicsContext::setTopology(const D3D12_PRIMITIVE_TOPOLOGY topology) const
{
	commandList->get()->IASetPrimitiveTopology(topology);
}

void GraphicsContext::setViewport(const float width, const float height)
{
	vp.Width = width;
	vp.Height = height;

	commandList->get()->RSSetViewports(1, &vp);
}

void GraphicsContext::setViewport(const UINT width, const UINT height)
{
	setViewport(static_cast<float>(width), static_cast<float>(height));
}

void GraphicsContext::setScissorRect(const UINT left, const UINT top, const UINT right, const UINT bottom)
{
	rt.left = left;
	rt.top = top;
	rt.right = right;
	rt.bottom = bottom;

	commandList->get()->RSSetScissorRects(1, &rt);
}

void GraphicsContext::setScissorRect(const float left, const float top, const float right, const float bottom)
{
	setScissorRect(static_cast<UINT>(left), static_cast<UINT>(top), static_cast<UINT>(right), static_cast<UINT>(bottom));
}

void GraphicsContext::setPipelineState(ID3D12PipelineState* const pipelineState) const
{
	commandList->get()->SetPipelineState(pipelineState);
}

void GraphicsContext::clearRenderTarget(const RenderTargetDesc desc, const FLOAT clearValue[4])
{
	commandList->get()->ClearRenderTargetView(desc.rtvHandle, clearValue, 0, nullptr);
}

void GraphicsContext::clearDepthStencil(const DepthStencilDesc desc, const D3D12_CLEAR_FLAGS flags, const FLOAT depth, const UINT8 stencil)
{
	commandList->get()->ClearDepthStencilView(desc.dsvHandle, flags, depth, stencil, 0, nullptr);
}

void GraphicsContext::uavBarrier(const std::initializer_list<Resource*>& resources)
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

	commandList->get()->ResourceBarrier(barriers.size(), barriers.data());
}

void GraphicsContext::draw(const UINT vertexCountPerInstance, const UINT instanceCount, const UINT startVertexLocation, const UINT startInstanceLocation)
{
	commandList->get()->DrawInstanced(vertexCountPerInstance, instanceCount, startVertexLocation, startInstanceLocation);
}

void GraphicsContext::begin()
{
	commandList->reset();

	commandList->setDescriptorHeap(GlobalDescriptorHeap::getResourceHeap(), GlobalDescriptorHeap::getSamplerHeap());

	commandList->setGraphicsRootSignature(GlobalRootSignature::getGraphicsRootSignature());

	commandList->setComputeRootSignature(GlobalRootSignature::getComputeRootSignature());

	commandList->get()->SetGraphicsRootConstantBufferView(0, globalConstantBuffer->getGPUAddress());

	commandList->get()->SetComputeRootConstantBufferView(0, globalConstantBuffer->getGPUAddress());
}

void GraphicsContext::end()
{
	commandList->get()->Close();
}

CommandList* GraphicsContext::getCommandList() const
{
	return commandList;
}

void GraphicsContext::updateReferredResStates()
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

void GraphicsContext::flushTransitionResources()
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
		commandList->get()->ResourceBarrier(transitionBarriers.size(), transitionBarriers.data());
	}

	transitionBarriers.clear();
}

void GraphicsContext::pushGraphicsStageIndexBuffer(UINT rootParameterIndex, const IndexConstantBuffer* const indexBuffer, const UINT targetSRVState)
{
	for (const ShaderResourceDesc& desc : indexBuffer->descs)
	{
		if (desc.type == ShaderResourceDesc::BUFFER)
		{
			Buffer* const buffer = desc.bufferDesc.buffer;

			if (buffer->getStateTracking() && desc.state != ShaderResourceDesc::CBV)
			{
				pushResourceTrackList(buffer);

				if (desc.state == ShaderResourceDesc::SRV)
				{
					if (buffer->transitionState != D3D12_RESOURCE_STATE_UNKNOWN)
					{
						buffer->transitionState = (buffer->transitionState | targetSRVState);
					}
					else
					{
						buffer->transitionState = targetSRVState;
					}
				}
				else if (desc.state == ShaderResourceDesc::UAV)
				{
					buffer->transitionState = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
				}
			}
		}
		else if (desc.type == ShaderResourceDesc::TEXTURE)
		{
			Texture* const texture = desc.textureDesc.texture;

			if (texture->getStateTracking())
			{
				pushResourceTrackList(texture);

				if (desc.state == ShaderResourceDesc::SRV)
				{
					if (desc.textureDesc.mipSlice == D3D12_TRANSITION_ALL_MIPLEVELS)
					{
						if (texture->transitionState.allState == D3D12_RESOURCE_STATE_UNKNOWN)
						{
							if (texture->mipLevels > 1)
							{
								for (UINT i = 0; i < texture->mipLevels; i++)
								{
									if (texture->transitionState.mipLevelStates[i] == D3D12_RESOURCE_STATE_UNKNOWN)
									{
										texture->transitionState.mipLevelStates[i] = targetSRVState;
									}
									else
									{
										texture->transitionState.mipLevelStates[i] = (texture->transitionState.mipLevelStates[i] | targetSRVState);
									}
								}
							}
							else
							{
								texture->transitionState.allState = targetSRVState;

								texture->transitionState.mipLevelStates[0] = targetSRVState;
							}
						}
						else
						{
							if (texture->transitionState.allState != targetSRVState)
							{
								texture->transitionState.allState = (texture->transitionState.allState | targetSRVState);

								for (UINT i = 0; i < texture->mipLevels; i++)
								{
									texture->transitionState.mipLevelStates[i] = (texture->transitionState.mipLevelStates[i] | targetSRVState);
								}
							}
						}
					}
					else
					{
						if (texture->mipLevels > 1)
						{
							if (texture->transitionState.mipLevelStates[desc.textureDesc.mipSlice] == D3D12_RESOURCE_STATE_UNKNOWN)
							{
								texture->transitionState.allState = D3D12_RESOURCE_STATE_UNKNOWN;

								texture->transitionState.mipLevelStates[desc.textureDesc.mipSlice] = targetSRVState;
							}
							else
							{
								if (texture->transitionState.mipLevelStates[desc.textureDesc.mipSlice] != targetSRVState)
								{
									texture->transitionState.allState = D3D12_RESOURCE_STATE_UNKNOWN;

									texture->transitionState.mipLevelStates[desc.textureDesc.mipSlice] = (texture->transitionState.mipLevelStates[desc.textureDesc.mipSlice] | targetSRVState);
								}
							}
						}
						else
						{
							if (texture->transitionState.mipLevelStates[desc.textureDesc.mipSlice] == D3D12_RESOURCE_STATE_UNKNOWN)
							{
								texture->transitionState.allState = targetSRVState;

								texture->transitionState.mipLevelStates[desc.textureDesc.mipSlice] = targetSRVState;
							}
							else
							{
								texture->transitionState.allState = (texture->transitionState.allState | targetSRVState);

								texture->transitionState.mipLevelStates[desc.textureDesc.mipSlice] = (texture->transitionState.mipLevelStates[desc.textureDesc.mipSlice] | targetSRVState);
							}
						}
					}
				}
				else if (desc.state == ShaderResourceDesc::UAV)
				{
					if (desc.textureDesc.mipSlice == D3D12_TRANSITION_ALL_MIPLEVELS)
					{
						texture->transitionState.allState = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;

						for (UINT i = 0; i < texture->mipLevels; i++)
						{
							texture->transitionState.mipLevelStates[i] = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
						}
					}
					else
					{
						if (texture->mipLevels > 1)
						{
							if (texture->transitionState.mipLevelStates[desc.textureDesc.mipSlice] != D3D12_RESOURCE_STATE_UNORDERED_ACCESS)
							{
								texture->transitionState.allState = D3D12_RESOURCE_STATE_UNKNOWN;

								texture->transitionState.mipLevelStates[desc.textureDesc.mipSlice] = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
							}
						}
						else
						{
							texture->transitionState.allState = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;

							texture->transitionState.mipLevelStates[desc.textureDesc.mipSlice] = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
						}
					}
				}
			}
		}
	}

	commandList->get()->SetGraphicsRootConstantBufferView(rootParameterIndex, indexBuffer->getGPUAddress());
}

void GraphicsContext::pushResourceTrackList(Texture* const texture)
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

void GraphicsContext::pushResourceTrackList(Buffer* const buffer)
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