#include<Gear/Core/RenderPass.h>

std::future<void> RenderPass::getPassResult()
{
	return std::async(std::launch::async, [&]
		{
			begin();

			recordCommand();

			end();
		});
}

RenderPass::RenderPass() :
	renderCMD(new CommandList(D3D12_COMMAND_LIST_TYPE_DIRECT)),
	transitionCMD(new CommandList(D3D12_COMMAND_LIST_TYPE_DIRECT))
{
}

RenderPass::~RenderPass()
{
	if (renderCMD)
	{
		delete renderCMD;
	}

	if (transitionCMD)
	{
		delete transitionCMD;
	}

	for (std::vector<Resource*>& transientPool : transientResources)
	{
		if (transientPool.size())
		{
			for (Resource* res : transientPool)
			{
				delete res;
			}
		}
	}
}

void RenderPass::setGraphicsGlobalIndexBuffer(const IndexConstantBuffer* const globalIndexBuffer)
{
	for (const TransitionDesc& desc : globalIndexBuffer->descs)
	{
		if (desc.type == TransitionDesc::BUFFER)
		{
			Buffer* const buffer = desc.buffer.buffer;

			if (buffer->getStateTracking())
			{
				pushResourceTrackList(buffer);

				if (desc.state == TransitionDesc::SRV)
				{
					buffer->transitionState = D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE;
				}
				else if (desc.state == TransitionDesc::UAV)
				{
					buffer->transitionState = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
				}
				else if (desc.state == TransitionDesc::CBV)
				{
					buffer->transitionState = D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
				}
			}
		}
		else if (desc.type == TransitionDesc::TEXTURE)
		{
			Texture* const texture = desc.texture.texture;

			if (texture->getStateTracking())
			{
				pushResourceTrackList(texture);

				if (desc.state == TransitionDesc::SRV)
				{
					if (desc.texture.mipSlice == D3D12_TRANSITION_ALL_MIPLEVELS)
					{
						texture->transitionState.allState = D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE;

						for (UINT i = 0; i < texture->mipLevels; i++)
						{
							texture->transitionState.mipLevelStates[i] = D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE;
						}
					}
					else
					{
						texture->transitionState.mipLevelStates[desc.texture.mipSlice] = D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE;

						if (texture->mipLevels == 1)
						{
							texture->transitionState.allState = D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE;
						}
					}
				}
				else if (desc.state == TransitionDesc::UAV)
				{
					if (desc.texture.mipSlice == D3D12_TRANSITION_ALL_MIPLEVELS)
					{
						texture->transitionState.allState = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;

						for (UINT i = 0; i < texture->mipLevels; i++)
						{
							texture->transitionState.mipLevelStates[i] = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
						}
					}
					else
					{
						texture->transitionState.mipLevelStates[desc.texture.mipSlice] = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;

						if (texture->mipLevels == 1)
						{
							texture->transitionState.allState = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
						}
					}
				}
			}
		}
	}

	Buffer* const buffer = globalIndexBuffer->buffer;

	if (buffer->getStateTracking())
	{
		pushResourceTrackList(buffer);

		buffer->transitionState = D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
	}

	flushTransitionResources();

	renderCMD->get()->SetGraphicsRootConstantBufferView(2, buffer->getGPUAddress());
}

void RenderPass::setComputeGlobalIndexBuffer(const IndexConstantBuffer* const globalIndexBuffer)
{
	for (const TransitionDesc& desc : globalIndexBuffer->descs)
	{
		if (desc.type == TransitionDesc::BUFFER)
		{
			Buffer* const buffer = desc.buffer.buffer;

			if (buffer->getStateTracking())
			{
				pushResourceTrackList(buffer);

				if (desc.state == TransitionDesc::SRV)
				{
					buffer->transitionState = D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
				}
				else if (desc.state == TransitionDesc::UAV)
				{
					buffer->transitionState = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
				}
				else if (desc.state == TransitionDesc::CBV)
				{
					buffer->transitionState = D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
				}
			}
		}
		else if (desc.type == TransitionDesc::TEXTURE)
		{
			Texture* const texture = desc.texture.texture;

			if (texture->getStateTracking())
			{
				pushResourceTrackList(texture);

				if (desc.state == TransitionDesc::SRV)
				{
					if (desc.texture.mipSlice == D3D12_TRANSITION_ALL_MIPLEVELS)
					{
						texture->transitionState.allState = D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;

						for (UINT i = 0; i < texture->mipLevels; i++)
						{
							texture->transitionState.mipLevelStates[i] = D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
						}
					}
					else
					{
						texture->transitionState.mipLevelStates[desc.texture.mipSlice] = D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;

						if (texture->mipLevels == 1)
						{
							texture->transitionState.allState = D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
						}
					}
				}
				else if (desc.state == TransitionDesc::UAV)
				{
					if (desc.texture.mipSlice == D3D12_TRANSITION_ALL_MIPLEVELS)
					{
						texture->transitionState.allState = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;

						for (UINT i = 0; i < texture->mipLevels; i++)
						{
							texture->transitionState.mipLevelStates[i] = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
						}
					}
					else
					{
						texture->transitionState.mipLevelStates[desc.texture.mipSlice] = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;

						if (texture->mipLevels == 1)
						{
							texture->transitionState.allState = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
						}
					}
				}
			}
		}
	}

	Buffer* const buffer = globalIndexBuffer->buffer;

	if (buffer->getStateTracking())
	{
		pushResourceTrackList(buffer);

		buffer->transitionState = D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
	}

	flushTransitionResources();

	renderCMD->get()->SetComputeRootConstantBufferView(2, buffer->getGPUAddress());
}

void RenderPass::setComputeIndexBuffer(const IndexConstantBuffer* const indexBuffer)
{
	for (const TransitionDesc& desc : indexBuffer->descs)
	{
		if (desc.type == TransitionDesc::BUFFER)
		{
			Buffer* const buffer = desc.buffer.buffer;

			if (buffer->getStateTracking())
			{
				pushResourceTrackList(buffer);

				if (desc.state == TransitionDesc::SRV)
				{
					buffer->transitionState = D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
				}
				else if (desc.state == TransitionDesc::UAV)
				{
					buffer->transitionState = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
				}
				else if (desc.state == TransitionDesc::CBV)
				{
					buffer->transitionState = D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
				}
			}
		}
		else if (desc.type == TransitionDesc::TEXTURE)
		{
			Texture* const texture = desc.texture.texture;

			if (texture->getStateTracking())
			{
				pushResourceTrackList(texture);

				if (desc.state == TransitionDesc::SRV)
				{
					if (desc.texture.mipSlice == D3D12_TRANSITION_ALL_MIPLEVELS)
					{
						texture->transitionState.allState = D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;

						for (UINT i = 0; i < texture->mipLevels; i++)
						{
							texture->transitionState.mipLevelStates[i] = D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
						}
					}
					else
					{
						texture->transitionState.mipLevelStates[desc.texture.mipSlice] = D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;

						if (texture->mipLevels == 1)
						{
							texture->transitionState.allState = D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
						}
					}
				}
				else if (desc.state == TransitionDesc::UAV)
				{
					if (desc.texture.mipSlice == D3D12_TRANSITION_ALL_MIPLEVELS)
					{
						texture->transitionState.allState = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;

						for (UINT i = 0; i < texture->mipLevels; i++)
						{
							texture->transitionState.mipLevelStates[i] = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
						}
					}
					else
					{
						texture->transitionState.mipLevelStates[desc.texture.mipSlice] = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;

						if (texture->mipLevels == 1)
						{
							texture->transitionState.allState = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
						}
					}
				}
			}
		}
	}

	Buffer* const buffer = indexBuffer->buffer;

	if (buffer->getStateTracking())
	{
		pushResourceTrackList(buffer);

		buffer->transitionState = D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
	}

	flushTransitionResources();

	renderCMD->get()->SetComputeRootConstantBufferView(3, buffer->getGPUAddress());
}

void RenderPass::setVertexIndexBuffer(const IndexConstantBuffer* const indexBuffer)
{
	pushGraphicsStageIndexBuffer(9, indexBuffer, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
}

void RenderPass::setHullIndexBuffer(const IndexConstantBuffer* const indexBuffer)
{
	pushGraphicsStageIndexBuffer(6, indexBuffer, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
}

void RenderPass::setGeometryIndexBuffer(const IndexConstantBuffer* const indexBuffer)
{
	pushGraphicsStageIndexBuffer(5, indexBuffer, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
}

void RenderPass::setDomainIndexBuffer(const IndexConstantBuffer* const indexBuffer)
{
	pushGraphicsStageIndexBuffer(4, indexBuffer, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
}

void RenderPass::setPixelIndexBuffer(const IndexConstantBuffer* const indexBuffer)
{
	pushGraphicsStageIndexBuffer(8, indexBuffer, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
}

void RenderPass::setMeshIndexBuffer(const IndexConstantBuffer* const indexBuffer)
{
	pushGraphicsStageIndexBuffer(7, indexBuffer, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
}

void RenderPass::setAmplificationIndexBuffer(const IndexConstantBuffer* const indexBuffer)
{
	pushGraphicsStageIndexBuffer(3, indexBuffer, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
}

void RenderPass::finishGraphicsStageIndexBuffer()
{
	flushTransitionResources();
}

void RenderPass::setRenderTargets(const std::initializer_list<TransitionDesc>& renderTargets,const std::initializer_list<TransitionDesc>& depthStencils)
{
	std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> rtvHandles;

	std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> dsvHandles;

	for (const TransitionDesc& desc : renderTargets)
	{
		rtvHandles.push_back(desc.texture.handle);

		Texture* const texture = desc.texture.texture;

		pushResourceTrackList(texture);

		if (texture->mipLevels > 1)
		{
			texture->transitionState.allState = D3D12_RESOURCE_STATE_UNKNOWN;

			texture->transitionState.mipLevelStates[desc.texture.mipSlice] = D3D12_RESOURCE_STATE_RENDER_TARGET;
		}
		else
		{
			texture->transitionState.allState = D3D12_RESOURCE_STATE_RENDER_TARGET;

			texture->transitionState.mipLevelStates[desc.texture.mipSlice] = D3D12_RESOURCE_STATE_RENDER_TARGET;
		}
	}

	//not support read only dsv currently
	for (const TransitionDesc& desc : depthStencils)
	{
		dsvHandles.push_back(desc.texture.handle);

		Texture* const texture = desc.texture.texture;

		pushResourceTrackList(texture);

		if (texture->mipLevels > 1)
		{
			texture->transitionState.allState = D3D12_RESOURCE_STATE_UNKNOWN;

			texture->transitionState.mipLevelStates[desc.texture.mipSlice] = D3D12_RESOURCE_STATE_DEPTH_WRITE;
		}
		else
		{
			texture->transitionState.allState = D3D12_RESOURCE_STATE_DEPTH_WRITE;

			texture->transitionState.mipLevelStates[desc.texture.mipSlice] = D3D12_RESOURCE_STATE_DEPTH_WRITE;
		}
	}

	flushTransitionResources();

	if (dsvHandles.size() > 0)
	{
		renderCMD->get()->OMSetRenderTargets(rtvHandles.size(), rtvHandles.data(), FALSE, dsvHandles.data());
	}
	else
	{
		renderCMD->get()->OMSetRenderTargets(rtvHandles.size(), rtvHandles.data(), FALSE, nullptr);
	}
}

void RenderPass::setVertexBuffers(const UINT startSlot,const std::initializer_list<VertexBuffer*>& vertexBuffers)
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

	renderCMD->get()->IASetVertexBuffers(startSlot, vbvs.size(), vbvs.data());
}

void RenderPass::setIndexBuffers(const std::initializer_list<IndexBuffer*>& indexBuffers)
{
	std::vector<D3D12_INDEX_BUFFER_VIEW> ibvs;

	for (IndexBuffer* const ib:indexBuffers)
	{
		Buffer* buffer = ib->getBuffer();

		pushResourceTrackList(buffer);

		buffer->transitionState = D3D12_RESOURCE_STATE_INDEX_BUFFER;

		ibvs.push_back(ib->getIndexBufferView());
	}

	flushTransitionResources();

	renderCMD->get()->IASetIndexBuffer(ibvs.data());
}

void RenderPass::setTopology(const D3D12_PRIMITIVE_TOPOLOGY topology)
{
	renderCMD->get()->IASetPrimitiveTopology(topology);
}

void RenderPass::updateBuffer(Buffer* const buffer, const void* const dataPtr, const UINT dataSize)
{
	pushResourceTrackList(buffer);

	buffer->transitionState = D3D12_RESOURCE_STATE_COPY_DEST;

	flushTransitionResources();

	buffer->update(dataPtr, dataSize);

	renderCMD->get()->CopyResource(buffer->getResource(), buffer->uploadHeaps[buffer->uploadHeapIndex]->getResource());

	buffer->uploadHeapIndex = (buffer->uploadHeapIndex + 1) % Graphics::FrameBufferCount;
}

void RenderPass::begin()
{
	for (Resource* res : transientResources[Graphics::getFrameIndex()])
	{
		delete res;
	}

	transientResources[Graphics::getFrameIndex()].clear();

	renderCMD->reset();

	renderCMD->setDescriptorHeap(GlobalDescriptorHeap::getResourceHeap(), GlobalDescriptorHeap::getSamplerHeap());

	renderCMD->setGraphicsRootSignature(GlobalRootSignature::getGraphicsRootSignature());

	renderCMD->setComputeRootSignature(GlobalRootSignature::getComputeRootSignature());
}

void RenderPass::end()
{
	renderCMD->get()->Close();
}

void RenderPass::updateReferredResStates()
{
	for (Resource* res : referredResources)
	{
		res->updateGlobalStates();

		if (res->isSharedResource())
		{
			res->resetInternalStates();
		}
	}

	referredResources.clear();
}

void RenderPass::flushTransitionResources()
{
	for (Buffer* buff : transitionBuffers)
	{
		buff->transition(transitionBarriers, pendingBufferBarrier);
	}

	transitionBuffers.clear();

	for (Texture* tex : transitionTextures)
	{
		tex->transition(transitionBarriers, pendingTextureBarrier);
	}

	transitionTextures.clear();

	renderCMD->get()->ResourceBarrier(transitionBarriers.size(), transitionBarriers.data());

	transitionBarriers.clear();
}

void RenderPass::pushGraphicsStageIndexBuffer(UINT rootParameterIndex, const IndexConstantBuffer* const indexBuffer, const UINT targetSRVState)
{
	for (const TransitionDesc& desc : indexBuffer->descs)
	{
		if (desc.type == TransitionDesc::BUFFER)
		{
			Buffer* const buffer = desc.buffer.buffer;

			if (buffer->getStateTracking())
			{
				pushResourceTrackList(buffer);

				if (desc.state == TransitionDesc::SRV)
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
				else if (desc.state == TransitionDesc::UAV)
				{
					buffer->transitionState = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
				}
				else if (desc.state == TransitionDesc::CBV)
				{
					buffer->transitionState = D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
				}
			}
		}
		else if (desc.type == TransitionDesc::TEXTURE)
		{
			Texture* const texture = desc.texture.texture;

			if (texture->getStateTracking())
			{
				pushResourceTrackList(texture);

				if (desc.state == TransitionDesc::SRV)
				{
					if (desc.texture.mipSlice == D3D12_TRANSITION_ALL_MIPLEVELS)
					{
						if (texture->transitionState.allState == D3D12_RESOURCE_STATE_UNKNOWN)
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
							texture->transitionState.allState = D3D12_RESOURCE_STATE_UNKNOWN;
						}

						if (texture->transitionState.mipLevelStates[desc.texture.mipSlice] == D3D12_RESOURCE_STATE_UNKNOWN)
						{
							texture->transitionState.mipLevelStates[desc.texture.mipSlice] = targetSRVState;

							if (texture->mipLevels == 1)
							{
								texture->transitionState.allState = targetSRVState;
							}
						}
						else
						{
							texture->transitionState.mipLevelStates[desc.texture.mipSlice] = (texture->transitionState.mipLevelStates[desc.texture.mipSlice] | targetSRVState);

							if (texture->mipLevels == 1)
							{
								texture->transitionState.allState = (texture->transitionState.allState | targetSRVState);
							}
						}
					}
				}
				else if (desc.state == TransitionDesc::UAV)
				{
					if (desc.texture.mipSlice == D3D12_TRANSITION_ALL_MIPLEVELS)
					{
						texture->transitionState.allState = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;

						for (UINT i = 0; i < texture->mipLevels; i++)
						{
							texture->transitionState.mipLevelStates[i] = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
						}
					}
					else
					{
						texture->transitionState.mipLevelStates[desc.texture.mipSlice] = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;

						if (texture->mipLevels == 1)
						{
							texture->transitionState.allState = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
						}
					}
				}
			}
		}
	}

	Buffer* const buffer = indexBuffer->buffer;

	if (buffer->getStateTracking())
	{
		pushResourceTrackList(buffer);

		buffer->transitionState = D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
	}

	renderCMD->get()->SetGraphicsRootConstantBufferView(rootParameterIndex, buffer->getGPUAddress());
}

void RenderPass::pushResourceTrackList(Texture* const texture)
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

void RenderPass::pushResourceTrackList(Buffer* const buffer)
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
