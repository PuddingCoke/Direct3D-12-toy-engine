#include<Gear/Core/GraphicsContext.h>

#include<Gear/Core/GlobalRootSignature.h>

ConstantBuffer* GraphicsContext::reservedGlobalConstantBuffer = nullptr;

GraphicsContext::GraphicsContext() :
	commandList(new CommandList(D3D12_COMMAND_LIST_TYPE_DIRECT)),
	vp{ 0.f,0.f,0.f,0.f,0.f,1.f },
	rt{ 0,0,0,0 },
	tempResourceIndices{}
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
	const BufferView::UpdateStruct updateStruct = bufferView->update(data, size);

	commandList->copyBufferRegion(updateStruct.buffer, 0, updateStruct.uploadHeap, 0, size);
}

void GraphicsContext::setGlobalConstantBuffer(const IndexConstantBuffer* const indexBuffer) const
{
	commandList->setAllPipelineResources(indexBuffer->getDescs());

	transitionResources();

	commandList->get()->SetGraphicsRootConstantBufferView(1, indexBuffer->getGPUAddress());

	commandList->get()->SetComputeRootConstantBufferView(1, indexBuffer->getGPUAddress());
}

void GraphicsContext::setGlobalConstantBuffer(const ConstantBuffer* const constantBuffer) const
{
	commandList->get()->SetGraphicsRootConstantBufferView(1, constantBuffer->getGPUAddress());

	commandList->get()->SetComputeRootConstantBufferView(1, constantBuffer->getGPUAddress());
}

void GraphicsContext::setVSConstants(const std::initializer_list<ShaderResourceDesc>& descs, const uint32_t offset)
{
	getIndicesFromResourceDescs(descs, tempResourceIndices);

	commandList->setGraphicsPipelineResources(descs, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);

	setVSConstants(static_cast<uint32_t>(descs.size()), tempResourceIndices, offset);
}

void GraphicsContext::setVSConstants(const uint32_t numValues, const void* const data, const uint32_t offset) const
{
	commandList->get()->SetGraphicsRoot32BitConstants(2, numValues, data, offset);
}

void GraphicsContext::setHSConstants(const std::initializer_list<ShaderResourceDesc>& descs, const uint32_t offset)
{
	getIndicesFromResourceDescs(descs, tempResourceIndices);

	commandList->setGraphicsPipelineResources(descs, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);

	setHSConstants(static_cast<uint32_t>(descs.size()), tempResourceIndices, offset);
}

void GraphicsContext::setHSConstants(const uint32_t numValues, const void* const data, const uint32_t offset) const
{
	commandList->get()->SetGraphicsRoot32BitConstants(3, numValues, data, offset);
}

void GraphicsContext::setDSConstants(const std::initializer_list<ShaderResourceDesc>& descs, const uint32_t offset)
{
	getIndicesFromResourceDescs(descs, tempResourceIndices);

	commandList->setGraphicsPipelineResources(descs, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);

	setDSConstants(static_cast<uint32_t>(descs.size()), tempResourceIndices, offset);
}

void GraphicsContext::setDSConstants(const uint32_t numValues, const void* const data, const uint32_t offset) const
{
	commandList->get()->SetGraphicsRoot32BitConstants(4, numValues, data, offset);
}

void GraphicsContext::setGSConstants(const std::initializer_list<ShaderResourceDesc>& descs, const uint32_t offset)
{
	getIndicesFromResourceDescs(descs, tempResourceIndices);

	commandList->setGraphicsPipelineResources(descs, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);

	setGSConstants(static_cast<uint32_t>(descs.size()), tempResourceIndices, offset);
}

void GraphicsContext::setGSConstants(const uint32_t numValues, const void* const data, const uint32_t offset) const
{
	commandList->get()->SetGraphicsRoot32BitConstants(5, numValues, data, offset);
}

void GraphicsContext::setPSConstants(const std::initializer_list<ShaderResourceDesc>& descs, const uint32_t offset)
{
	getIndicesFromResourceDescs(descs, tempResourceIndices);

	commandList->setGraphicsPipelineResources(descs, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

	setPSConstants(static_cast<uint32_t>(descs.size()), tempResourceIndices, offset);
}

void GraphicsContext::setPSConstants(const uint32_t numValues, const void* const data, const uint32_t offset) const
{
	commandList->get()->SetGraphicsRoot32BitConstants(6, numValues, data, offset);
}

void GraphicsContext::setCSConstants(const std::initializer_list<ShaderResourceDesc>& descs, const uint32_t offset)
{
	getIndicesFromResourceDescs(descs, tempResourceIndices);

	commandList->setComputePipelineResources(descs);

	setCSConstants(static_cast<uint32_t>(descs.size()), tempResourceIndices, offset);
}

void GraphicsContext::setCSConstants(const uint32_t numValues, const void* const data, const uint32_t offset) const
{
	commandList->get()->SetComputeRoot32BitConstants(2, numValues, data, offset);
}

void GraphicsContext::setVSConstantBuffer(const IndexConstantBuffer* const constantBuffer)
{
	commandList->setGraphicsPipelineResources(constantBuffer->getDescs(), D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);

	setVSConstantBuffer(constantBuffer->getConstantBuffer());
}

void GraphicsContext::setVSConstantBuffer(const ConstantBuffer* const constantBuffer) const
{
	commandList->get()->SetGraphicsRootConstantBufferView(7, constantBuffer->getGPUAddress());
}

void GraphicsContext::setHSConstantBuffer(const IndexConstantBuffer* const constantBuffer)
{
	commandList->setGraphicsPipelineResources(constantBuffer->getDescs(), D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);

	setHSConstantBuffer(constantBuffer->getConstantBuffer());
}

void GraphicsContext::setHSConstantBuffer(const ConstantBuffer* const constantBuffer) const
{
	commandList->get()->SetGraphicsRootConstantBufferView(8, constantBuffer->getGPUAddress());
}

void GraphicsContext::setDSConstantBuffer(const IndexConstantBuffer* const constantBuffer)
{
	commandList->setGraphicsPipelineResources(constantBuffer->getDescs(), D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);

	setDSConstantBuffer(constantBuffer->getConstantBuffer());
}

void GraphicsContext::setDSConstantBuffer(const ConstantBuffer* const constantBuffer) const
{
	commandList->get()->SetGraphicsRootConstantBufferView(9, constantBuffer->getGPUAddress());
}

void GraphicsContext::setGSConstantBuffer(const IndexConstantBuffer* const constantBuffer)
{
	commandList->setGraphicsPipelineResources(constantBuffer->getDescs(), D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);

	setGSConstantBuffer(constantBuffer->getConstantBuffer());
}

void GraphicsContext::setGSConstantBuffer(const ConstantBuffer* const constantBuffer) const
{
	commandList->get()->SetGraphicsRootConstantBufferView(10, constantBuffer->getGPUAddress());
}

void GraphicsContext::setPSConstantBuffer(const IndexConstantBuffer* const constantBuffer)
{
	commandList->setGraphicsPipelineResources(constantBuffer->getDescs(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

	setPSConstantBuffer(constantBuffer->getConstantBuffer());
}

void GraphicsContext::setPSConstantBuffer(const ConstantBuffer* const constantBuffer) const
{
	commandList->get()->SetGraphicsRootConstantBufferView(11, constantBuffer->getGPUAddress());
}

void GraphicsContext::setCSConstantBuffer(const IndexConstantBuffer* const constantBuffer)
{
	commandList->setComputePipelineResources(constantBuffer->getDescs());

	setCSConstantBuffer(constantBuffer->getConstantBuffer());
}

void GraphicsContext::setCSConstantBuffer(const ConstantBuffer* const constantBuffer) const
{
	commandList->get()->SetComputeRootConstantBufferView(3, constantBuffer->getGPUAddress());
}


void GraphicsContext::transitionResources() const
{
	commandList->transitionResources();
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

void GraphicsContext::drawIndexed(const uint32_t indexCountPerInstance, const uint32_t instanceCount, const uint32_t startIndexLocation, const int baseVertexLocation, const uint32_t startInstanceLocation) const
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

	commandList->get()->SetGraphicsRootConstantBufferView(0, reservedGlobalConstantBuffer->getGPUAddress());

	commandList->get()->SetComputeRootConstantBufferView(0, reservedGlobalConstantBuffer->getGPUAddress());
}

void GraphicsContext::end() const
{
	commandList->close();
}

CommandList* GraphicsContext::getCommandList() const
{
	return commandList;
}

void GraphicsContext::setReservedGlobalConstantBuffer(ConstantBuffer* const buffer)
{
	reservedGlobalConstantBuffer = buffer;
}

void GraphicsContext::getIndicesFromResourceDescs(const std::initializer_list<ShaderResourceDesc>& descs, uint32_t* const dst) const
{
	uint32_t index = 0;

	for (const ShaderResourceDesc& desc : descs)
	{
		dst[index] = desc.resourceIndex;

		index++;
	}
}