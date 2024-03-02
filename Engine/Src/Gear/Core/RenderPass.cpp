#include<Gear/Core/RenderPass.h>

ConstantBuffer* RenderPass::globalConstantBuffer = nullptr;

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
	transitionCMD(new CommandList(D3D12_COMMAND_LIST_TYPE_DIRECT)),
	vp{ 0.f,0.f,0.f,0.f,0.f,1.f },
	rt{ 0,0,0,0 }
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

ConstantBuffer* RenderPass::CreateConstantBuffer(const UINT size, const bool cpuWritable, const void* const data)
{
	return new ConstantBuffer(size, cpuWritable, data, renderCMD->get(), &transientResources[Graphics::getFrameIndex()]);
}

IndexBuffer* RenderPass::CreateIndexBuffer(const DXGI_FORMAT format, const UINT size, const bool cpuWritable, const void* const data)
{
	const bool stateTracking = cpuWritable;

	return new IndexBuffer(format, size, stateTracking, cpuWritable, data, renderCMD->get(), &transientResources[Graphics::getFrameIndex()]);
}

IndexConstantBuffer* RenderPass::CreateIndexConstantBuffer(const std::initializer_list<ShaderResourceDesc>& descs, const bool cpuWritable)
{
	return new IndexConstantBuffer(descs, cpuWritable, renderCMD->get(), &transientResources[Graphics::getFrameIndex()]);
}

IndexConstantBuffer* RenderPass::CreateIndexConstantBuffer(const UINT indicesNum, const bool cpuWritable)
{
	return new IndexConstantBuffer(indicesNum, cpuWritable);
}

TextureDepthStencil* RenderPass::CreateTextureDepthStencil(const UINT width, const UINT height, const DXGI_FORMAT resFormat, const UINT arraySize, const UINT mipLevels, const bool isTextureCube)
{
	return new TextureDepthStencil(width, height, resFormat, arraySize, mipLevels, isTextureCube);
}

TextureRenderTarget* RenderPass::CreateTextureRenderTarget(const TextureViewCreationFlags flags, const UINT width, const UINT height, const DXGI_FORMAT resFormat, const UINT arraySize, const UINT mipLevels, const DXGI_FORMAT srvFormat, const DXGI_FORMAT uavFormat, const DXGI_FORMAT rtvFormat, const bool isTextureCube)
{
	return new TextureRenderTarget(flags, width, height, resFormat, arraySize, mipLevels, srvFormat, uavFormat, rtvFormat, isTextureCube);
}

TextureRenderTarget* RenderPass::CreateTextureRenderTarget(const TextureViewCreationFlags flags, const std::string filePath, const DXGI_FORMAT srvFormat, const DXGI_FORMAT uavFormat, const DXGI_FORMAT rtvFormat, const bool isTextureCube)
{
	return new TextureRenderTarget(flags, filePath, renderCMD->get(), &transientResources[Graphics::getFrameIndex()], srvFormat, uavFormat, rtvFormat, isTextureCube);
}

VertexBuffer* RenderPass::CreateVertexBuffer(const UINT perVertexSize, const UINT size, const bool stateTracking, const bool cpuWritable, const void* const data)
{
	return new VertexBuffer(perVertexSize, size, stateTracking, cpuWritable, data, renderCMD->get(), &transientResources[Graphics::getFrameIndex()]);
}

void RenderPass::setGlobalIndexBuffer(const IndexConstantBuffer* const indexBuffer)
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

	renderCMD->get()->SetGraphicsRootConstantBufferView(2, indexBuffer->getGPUAddress());
	renderCMD->get()->SetComputeRootConstantBufferView(2, indexBuffer->getGPUAddress());
}

void RenderPass::setComputeIndexBuffer(const IndexConstantBuffer* const indexBuffer)
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

	renderCMD->get()->SetComputeRootConstantBufferView(3, indexBuffer->getGPUAddress());
}

void RenderPass::setGraphicsConstants(const UINT numValues, const void* const data, const UINT offset)
{
	renderCMD->get()->SetGraphicsRoot32BitConstants(1, numValues, data, offset);
}

void RenderPass::setComputeConstants(const UINT numValues, const void* const data, const UINT offset)
{
	renderCMD->get()->SetComputeRoot32BitConstants(1, numValues, data, offset);
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

void RenderPass::setRenderTargets(const std::initializer_list<RenderTargetDesc>& renderTargets, const std::initializer_list<DepthStencilDesc>& depthStencils)
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
		renderCMD->get()->OMSetRenderTargets(rtvHandles.size(), rtvHandles.data(), FALSE, dsvHandles.data());
	}
	else
	{
		renderCMD->get()->OMSetRenderTargets(rtvHandles.size(), rtvHandles.data(), FALSE, nullptr);
	}
}

void RenderPass::setDefRenderTarget()
{
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = Graphics::getBackBufferHandle();

	renderCMD->get()->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);
}

void RenderPass::clearDefRenderTarget(const FLOAT clearValue[4])
{
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = Graphics::getBackBufferHandle();

	renderCMD->get()->ClearRenderTargetView(rtvHandle, clearValue, 0, nullptr);
}

void RenderPass::setVertexBuffers(const UINT startSlot, const std::initializer_list<VertexBuffer*>& vertexBuffers)
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

	for (IndexBuffer* const ib : indexBuffers)
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

void RenderPass::setViewport(const float width, const float height)
{
	vp.Width = width;
	vp.Height = height;

	renderCMD->get()->RSSetViewports(1, &vp);
}

void RenderPass::setViewport(const UINT width, const UINT height)
{
	setViewport(static_cast<float>(width), static_cast<float>(height));
}

void RenderPass::setScissorRect(const UINT left, const UINT top, const UINT right, const UINT bottom)
{
	rt.left = left;
	rt.top = top;
	rt.right = right;
	rt.bottom = bottom;

	renderCMD->get()->RSSetScissorRects(1, &rt);
}

void RenderPass::setScissorRect(const float left, const float top, const float right, const float bottom)
{
	setScissorRect(static_cast<UINT>(left), static_cast<UINT>(top), static_cast<UINT>(right), static_cast<UINT>(bottom));
}

void RenderPass::setPipelineState(ID3D12PipelineState* const pipelineState)
{
	renderCMD->get()->SetPipelineState(pipelineState);
}

void RenderPass::clearRenderTarget(const RenderTargetDesc desc, const FLOAT clearValue[4])
{
	renderCMD->get()->ClearRenderTargetView(desc.rtvHandle, clearValue, 0, nullptr);
}

void RenderPass::clearDepthStencil(const DepthStencilDesc desc, const D3D12_CLEAR_FLAGS flags, const FLOAT depth, const UINT8 stencil)
{
	renderCMD->get()->ClearDepthStencilView(desc.dsvHandle, flags, depth, stencil, 0, nullptr);
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

	renderCMD->get()->SetGraphicsRootConstantBufferView(0, globalConstantBuffer->getGPUAddress());

	renderCMD->get()->SetComputeRootConstantBufferView(0, globalConstantBuffer->getGPUAddress());
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

	renderCMD->get()->SetGraphicsRootConstantBufferView(rootParameterIndex, indexBuffer->getGPUAddress());
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
