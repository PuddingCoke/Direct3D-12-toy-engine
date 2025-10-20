#include<Gear/Core/GraphicsContext.h>

#include<Gear/Core/GlobalRootSignature.h>

ImmutableCBuffer* GraphicsContext::reservedGlobalCBuffer = nullptr;

GraphicsContext::GraphicsContext() :
	commandList(new CommandList(D3D12_COMMAND_LIST_TYPE_DIRECT)),
	vp{ 0.f,0.f,0.f,0.f,0.f,1.f },
	rt{ 0,0,0,0 },
	resourceIndices{}
{
}

GraphicsContext::~GraphicsContext()
{
	if (commandList)
	{
		delete commandList;
	}
}

void GraphicsContext::updateBuffer(BufferView* const bufferView, const void* const data, const uint32_t size) const
{
	const BufferView::UpdateStruct updateStruct = bufferView->getUpdateStruct(data, size);

	commandList->copyBufferRegion(updateStruct.buffer, 0, updateStruct.uploadHeap, 0, size);
}

void GraphicsContext::updateBuffer(StaticCBuffer* const staticCBuffer, const void* const data, const uint32_t size) const
{
	const StaticCBuffer::UpdateStruct updateStruct = staticCBuffer->getUpdateStruct(data, size);

	commandList->copyBufferRegion(updateStruct.buffer, 0, updateStruct.uploadHeap, 0, size);
}

void GraphicsContext::setVSConstants(const std::initializer_list<ShaderResourceDesc>& descs, const uint32_t offset)
{
	getResourceIndicesFromDescs(descs);

	commandList->setGraphicsPipelineResources(descs, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);

	setVSConstants(static_cast<uint32_t>(descs.size()), resourceIndices, offset);
}

void GraphicsContext::setVSConstants(const uint32_t numValues, const void* const data, const uint32_t offset) const
{
	commandList->get()->SetGraphicsRoot32BitConstants(2, numValues, data, offset);
}

void GraphicsContext::setHSConstants(const std::initializer_list<ShaderResourceDesc>& descs, const uint32_t offset)
{
	getResourceIndicesFromDescs(descs);

	commandList->setGraphicsPipelineResources(descs, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);

	setHSConstants(static_cast<uint32_t>(descs.size()), resourceIndices, offset);
}

void GraphicsContext::setHSConstants(const uint32_t numValues, const void* const data, const uint32_t offset) const
{
	commandList->get()->SetGraphicsRoot32BitConstants(3, numValues, data, offset);
}

void GraphicsContext::setDSConstants(const std::initializer_list<ShaderResourceDesc>& descs, const uint32_t offset)
{
	getResourceIndicesFromDescs(descs);

	commandList->setGraphicsPipelineResources(descs, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);

	setDSConstants(static_cast<uint32_t>(descs.size()), resourceIndices, offset);
}

void GraphicsContext::setDSConstants(const uint32_t numValues, const void* const data, const uint32_t offset) const
{
	commandList->get()->SetGraphicsRoot32BitConstants(4, numValues, data, offset);
}

void GraphicsContext::setGSConstants(const std::initializer_list<ShaderResourceDesc>& descs, const uint32_t offset)
{
	getResourceIndicesFromDescs(descs);

	commandList->setGraphicsPipelineResources(descs, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);

	setGSConstants(static_cast<uint32_t>(descs.size()), resourceIndices, offset);
}

void GraphicsContext::setGSConstants(const uint32_t numValues, const void* const data, const uint32_t offset) const
{
	commandList->get()->SetGraphicsRoot32BitConstants(5, numValues, data, offset);
}

void GraphicsContext::setPSConstants(const std::initializer_list<ShaderResourceDesc>& descs, const uint32_t offset)
{
	getResourceIndicesFromDescs(descs);

	commandList->setGraphicsPipelineResources(descs, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

	setPSConstants(static_cast<uint32_t>(descs.size()), resourceIndices, offset);
}

void GraphicsContext::setPSConstants(const uint32_t numValues, const void* const data, const uint32_t offset) const
{
	commandList->get()->SetGraphicsRoot32BitConstants(6, numValues, data, offset);
}

void GraphicsContext::setCSConstants(const std::initializer_list<ShaderResourceDesc>& descs, const uint32_t offset)
{
	getResourceIndicesFromDescs(descs);

	commandList->setComputePipelineResources(descs);

	setCSConstants(static_cast<uint32_t>(descs.size()), resourceIndices, offset);
}

void GraphicsContext::setCSConstants(const uint32_t numValues, const void* const data, const uint32_t offset) const
{
	commandList->get()->SetComputeRoot32BitConstants(2, numValues, data, offset);
}

void GraphicsContext::setGlobalConstantBuffer(const ImmutableCBuffer* const immutableCBuffer)
{
	Buffer* const buffer = immutableCBuffer->getBuffer();

	if (buffer && buffer->getStateTracking())
	{
		commandList->pushResourceTrackList(buffer);

		buffer->setState(D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
	}

	pushRootConstantBufferDesc({ RootConstantBufferDesc::GRAPHICS,1,immutableCBuffer->getGPUAddress() });

	pushRootConstantBufferDesc({ RootConstantBufferDesc::COMPUTE,1,immutableCBuffer->getGPUAddress() });
}

void GraphicsContext::setVSConstantBuffer(const ImmutableCBuffer* const immutableCBuffer)
{
	Buffer* const buffer = immutableCBuffer->getBuffer();

	if (buffer && buffer->getStateTracking())
	{
		commandList->pushResourceTrackList(buffer);

		buffer->setState(D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
	}

	pushRootConstantBufferDesc({ RootConstantBufferDesc::GRAPHICS,7,immutableCBuffer->getGPUAddress() });
}

void GraphicsContext::setHSConstantBuffer(const ImmutableCBuffer* const immutableCBuffer)
{
	Buffer* const buffer = immutableCBuffer->getBuffer();

	if (buffer && buffer->getStateTracking())
	{
		commandList->pushResourceTrackList(buffer);

		buffer->setState(D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
	}

	pushRootConstantBufferDesc({ RootConstantBufferDesc::GRAPHICS,8,immutableCBuffer->getGPUAddress() });
}

void GraphicsContext::setDSConstantBuffer(const ImmutableCBuffer* const immutableCBuffer)
{
	Buffer* const buffer = immutableCBuffer->getBuffer();

	if (buffer && buffer->getStateTracking())
	{
		commandList->pushResourceTrackList(buffer);

		buffer->setState(D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
	}

	pushRootConstantBufferDesc({ RootConstantBufferDesc::GRAPHICS,9,immutableCBuffer->getGPUAddress() });
}

void GraphicsContext::setGSConstantBuffer(const ImmutableCBuffer* const immutableCBuffer)
{
	Buffer* const buffer = immutableCBuffer->getBuffer();

	if (buffer && buffer->getStateTracking())
	{
		commandList->pushResourceTrackList(buffer);

		buffer->setState(D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
	}

	pushRootConstantBufferDesc({ RootConstantBufferDesc::GRAPHICS,10,immutableCBuffer->getGPUAddress() });
}

void GraphicsContext::setPSConstantBuffer(const ImmutableCBuffer* const immutableCBuffer)
{
	Buffer* const buffer = immutableCBuffer->getBuffer();

	if (buffer && buffer->getStateTracking())
	{
		commandList->pushResourceTrackList(buffer);

		buffer->setState(D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
	}

	pushRootConstantBufferDesc({ RootConstantBufferDesc::GRAPHICS,11,immutableCBuffer->getGPUAddress() });
}

void GraphicsContext::setCSConstantBuffer(const ImmutableCBuffer* const immutableCBuffer)
{
	Buffer* const buffer = immutableCBuffer->getBuffer();

	if (buffer && buffer->getStateTracking())
	{
		commandList->pushResourceTrackList(buffer);

		buffer->setState(D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
	}

	pushRootConstantBufferDesc({ RootConstantBufferDesc::COMPUTE,3,immutableCBuffer->getGPUAddress() });
}

void GraphicsContext::transitionResources()
{
	commandList->transitionResources();

	if (rootConstantBufferDescs.size())
	{
		for (uint32_t i = 0; i < rootConstantBufferDescs.size(); i++)
		{
			if (rootConstantBufferDescs[i].type == RootConstantBufferDesc::GRAPHICS)
			{
				const uint32_t rootParameterIndex = rootConstantBufferDescs[i].rootParameterIndex;

				const D3D12_GPU_VIRTUAL_ADDRESS gpuAddress = rootConstantBufferDescs[i].gpuAddress;

				commandList->get()->SetGraphicsRootConstantBufferView(rootParameterIndex, gpuAddress);
			}
			else
			{
				const uint32_t rootParameterIndex = rootConstantBufferDescs[i].rootParameterIndex;

				const D3D12_GPU_VIRTUAL_ADDRESS gpuAddress = rootConstantBufferDescs[i].gpuAddress;

				commandList->get()->SetComputeRootConstantBufferView(rootParameterIndex, gpuAddress);
			}
		}

		rootConstantBufferDescs.clear();
	}
}

void GraphicsContext::setRenderTargets(const std::initializer_list<RenderTargetDesc>& renderTargets, const DepthStencilDesc* const depthStencils) const
{
	commandList->setRenderTargets(renderTargets, depthStencils);
}

void GraphicsContext::setDefRenderTarget() const
{
	commandList->setDefRenderTarget();
}

void GraphicsContext::clearDefRenderTarget(const float clearValue[4]) const
{
	commandList->clearDefRenderTarget(clearValue);
}

void GraphicsContext::setVertexBuffers(const uint32_t startSlot, const std::initializer_list<VertexBufferDesc>& vertexBuffers) const
{
	commandList->setVertexBuffers(startSlot, vertexBuffers);
}

void GraphicsContext::setIndexBuffer(const IndexBufferDesc& indexBuffer) const
{
	commandList->setIndexBuffer(indexBuffer);
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

void GraphicsContext::setViewport(const uint32_t width, const uint32_t height)
{
	setViewport(static_cast<float>(width), static_cast<float>(height));
}

void GraphicsContext::setScissorRect(const uint32_t left, const uint32_t top, const uint32_t right, const uint32_t bottom)
{
	rt.left = left;
	rt.top = top;
	rt.right = right;
	rt.bottom = bottom;

	commandList->get()->RSSetScissorRects(1, &rt);
}

void GraphicsContext::setScissorRect(const float left, const float top, const float right, const float bottom)
{
	setScissorRect(static_cast<uint32_t>(left), static_cast<uint32_t>(top), static_cast<uint32_t>(right), static_cast<uint32_t>(bottom));
}

void GraphicsContext::setViewportSimple(const float width, const float height)
{
	setViewport(width, height);
	setScissorRect(0, 0, width, height);
}

void GraphicsContext::setViewportSimple(const uint32_t width, const uint32_t height)
{
	setViewport(width, height);
	setScissorRect(0, 0, width, height);
}

void GraphicsContext::setPipelineState(ID3D12PipelineState* const pipelineState) const
{
	commandList->get()->SetPipelineState(pipelineState);
}

void GraphicsContext::clearRenderTarget(const RenderTargetDesc& desc, const float clearValue[4]) const
{
	commandList->get()->ClearRenderTargetView(desc.rtvHandle, clearValue, 0, nullptr);
}

void GraphicsContext::clearDepthStencil(const DepthStencilDesc& desc, const D3D12_CLEAR_FLAGS flags, const float depth, const uint8_t stencil) const
{
	commandList->get()->ClearDepthStencilView(desc.dsvHandle, flags, depth, stencil, 0, nullptr);
}

void GraphicsContext::clearUnorderedAccess(const ClearUAVDesc& desc, const float values[4])
{
	commandList->clearUnorderedAccessView(desc, values);
}

void GraphicsContext::clearUnorderedAccess(const ClearUAVDesc& desc, const uint32_t values[4])
{
	commandList->clearUnorderedAccessView(desc, values);
}

void GraphicsContext::uavBarrier(const std::initializer_list<Resource*>& resources) const
{
	commandList->uavBarrier(resources);
}

void GraphicsContext::draw(const uint32_t vertexCountPerInstance, const uint32_t instanceCount, const uint32_t startVertexLocation, const uint32_t startInstanceLocation) const
{
	commandList->get()->DrawInstanced(vertexCountPerInstance, instanceCount, startVertexLocation, startInstanceLocation);
}

void GraphicsContext::drawIndexed(const uint32_t indexCountPerInstance, const uint32_t instanceCount, const uint32_t startIndexLocation, const int32_t baseVertexLocation, const uint32_t startInstanceLocation) const
{
	commandList->get()->DrawIndexedInstanced(indexCountPerInstance, instanceCount, startIndexLocation, baseVertexLocation, startInstanceLocation);
}

void GraphicsContext::dispatch(const uint32_t threadGroupCountX, const uint32_t threadGroupCountY, const uint32_t threadGroupCountZ) const
{
	commandList->get()->Dispatch(threadGroupCountX, threadGroupCountY, threadGroupCountZ);
}

void GraphicsContext::begin() const
{
	commandList->open();

	commandList->setDescriptorHeap(GlobalDescriptorHeap::getResourceHeap(), GlobalDescriptorHeap::getSamplerHeap());

	commandList->setGraphicsRootSignature(GlobalRootSignature::getGraphicsRootSignature());

	commandList->setComputeRootSignature(GlobalRootSignature::getComputeRootSignature());

	commandList->get()->SetGraphicsRootConstantBufferView(0, reservedGlobalCBuffer->getGPUAddress());

	commandList->get()->SetComputeRootConstantBufferView(0, reservedGlobalCBuffer->getGPUAddress());
}

CommandList* GraphicsContext::getCommandList() const
{
	return commandList;
}

void GraphicsContext::getResourceIndicesFromDescs(const std::initializer_list<ShaderResourceDesc>& descs)
{
	std::transform(descs.begin(), descs.end(), resourceIndices,
		[](const ShaderResourceDesc& desc)
		{
			return desc.resourceIndex;
		});
}

void GraphicsContext::pushRootConstantBufferDesc(const RootConstantBufferDesc& desc)
{
	rootConstantBufferDescs.emplace_back(desc);
}
