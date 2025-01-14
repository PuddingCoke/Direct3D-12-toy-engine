#include<Gear/Core/GraphicsContext.h>

ConstantBuffer* GraphicsContext::globalConstantBuffer = nullptr;

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

void GraphicsContext::updateBuffer(BufferView* const bufferView, const void* const data, const UINT size)
{
	Buffer* const buffer = bufferView->buffer;

	UploadHeap* const uploadHeap = bufferView->uploadHeaps[bufferView->uploadHeapIndex];

	uploadHeap->update(data, size);

	commandList->copyBufferRegion(buffer, 0, uploadHeap, 0, size);

	bufferView->uploadHeapIndex = (bufferView->uploadHeapIndex + 1) % Graphics::getFrameBufferCount();
}

void GraphicsContext::setGlobalConstantBuffer(const IndexConstantBuffer* const indexBuffer)
{
	commandList->setAllPipelineResources(indexBuffer->descs);

	transitionResources();

	commandList->get()->SetGraphicsRootConstantBufferView(1, indexBuffer->getGPUAddress());
	commandList->get()->SetComputeRootConstantBufferView(1, indexBuffer->getGPUAddress());
}

void GraphicsContext::setGlobalConstantBuffer(const ConstantBuffer* const constantBuffer)
{
	commandList->get()->SetGraphicsRootConstantBufferView(1, constantBuffer->getGPUAddress());
	commandList->get()->SetComputeRootConstantBufferView(1, constantBuffer->getGPUAddress());
}

void GraphicsContext::setVSConstants(const std::initializer_list<ShaderResourceDesc>& descs, const UINT offset)
{
	getIndicesFromResourceDescs(descs, tempResourceIndices);

	commandList->setGraphicsPipelineResources(descs, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);

	setVSConstants(static_cast<UINT>(descs.size()), tempResourceIndices, offset);
}

void GraphicsContext::setVSConstants(const UINT numValues, const void* const data, const UINT offset)
{
	commandList->get()->SetGraphicsRoot32BitConstants(2, numValues, data, offset);
}

void GraphicsContext::setHSConstants(const std::initializer_list<ShaderResourceDesc>& descs, const UINT offset)
{
	getIndicesFromResourceDescs(descs, tempResourceIndices);

	commandList->setGraphicsPipelineResources(descs, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);

	setHSConstants(static_cast<UINT>(descs.size()), tempResourceIndices, offset);
}

void GraphicsContext::setHSConstants(const UINT numValues, const void* const data, const UINT offset)
{
	commandList->get()->SetGraphicsRoot32BitConstants(3, numValues, data, offset);
}

void GraphicsContext::setDSConstants(const std::initializer_list<ShaderResourceDesc>& descs, const UINT offset)
{
	getIndicesFromResourceDescs(descs, tempResourceIndices);

	commandList->setGraphicsPipelineResources(descs, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);

	setDSConstants(static_cast<UINT>(descs.size()), tempResourceIndices, offset);
}

void GraphicsContext::setDSConstants(const UINT numValues, const void* const data, const UINT offset)
{
	commandList->get()->SetGraphicsRoot32BitConstants(4, numValues, data, offset);
}

void GraphicsContext::setGSConstants(const std::initializer_list<ShaderResourceDesc>& descs, const UINT offset)
{
	getIndicesFromResourceDescs(descs, tempResourceIndices);

	commandList->setGraphicsPipelineResources(descs, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);

	setGSConstants(static_cast<UINT>(descs.size()), tempResourceIndices, offset);
}

void GraphicsContext::setGSConstants(const UINT numValues, const void* const data, const UINT offset)
{
	commandList->get()->SetGraphicsRoot32BitConstants(5, numValues, data, offset);
}

void GraphicsContext::setPSConstants(const std::initializer_list<ShaderResourceDesc>& descs, const UINT offset)
{
	getIndicesFromResourceDescs(descs, tempResourceIndices);

	commandList->setGraphicsPipelineResources(descs, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

	setPSConstants(static_cast<UINT>(descs.size()), tempResourceIndices, offset);
}

void GraphicsContext::setPSConstants(const UINT numValues, const void* const data, const UINT offset)
{
	commandList->get()->SetGraphicsRoot32BitConstants(6, numValues, data, offset);
}

void GraphicsContext::setCSConstants(const std::initializer_list<ShaderResourceDesc>& descs, const UINT offset)
{
	getIndicesFromResourceDescs(descs, tempResourceIndices);

	commandList->setComputePipelineResources(descs);

	setCSConstants(static_cast<UINT>(descs.size()), tempResourceIndices, offset);
}

void GraphicsContext::setCSConstants(const UINT numValues, const void* const data, const UINT offset)
{
	commandList->get()->SetComputeRoot32BitConstants(2, numValues, data, offset);
}

void GraphicsContext::setVSConstantBuffer(const IndexConstantBuffer* const constantBuffer)
{
	commandList->setGraphicsPipelineResources(constantBuffer->descs, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);

	setVSConstantBuffer(constantBuffer->constantBuffer);
}

void GraphicsContext::setVSConstantBuffer(const ConstantBuffer* const constantBuffer)
{
	commandList->get()->SetGraphicsRootConstantBufferView(7, constantBuffer->getGPUAddress());
}

void GraphicsContext::setHSConstantBuffer(const IndexConstantBuffer* const constantBuffer)
{
	commandList->setGraphicsPipelineResources(constantBuffer->descs, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);

	setHSConstantBuffer(constantBuffer->constantBuffer);
}

void GraphicsContext::setHSConstantBuffer(const ConstantBuffer* const constantBuffer)
{
	commandList->get()->SetGraphicsRootConstantBufferView(8, constantBuffer->getGPUAddress());
}

void GraphicsContext::setDSConstantBuffer(const IndexConstantBuffer* const constantBuffer)
{
	commandList->setGraphicsPipelineResources(constantBuffer->descs, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);

	setDSConstantBuffer(constantBuffer->constantBuffer);
}

void GraphicsContext::setDSConstantBuffer(const ConstantBuffer* const constantBuffer)
{
	commandList->get()->SetGraphicsRootConstantBufferView(9, constantBuffer->getGPUAddress());
}

void GraphicsContext::setGSConstantBuffer(const IndexConstantBuffer* const constantBuffer)
{
	commandList->setGraphicsPipelineResources(constantBuffer->descs, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);

	setGSConstantBuffer(constantBuffer->constantBuffer);
}

void GraphicsContext::setGSConstantBuffer(const ConstantBuffer* const constantBuffer)
{
	commandList->get()->SetGraphicsRootConstantBufferView(10, constantBuffer->getGPUAddress());
}

void GraphicsContext::setPSConstantBuffer(const IndexConstantBuffer* const constantBuffer)
{
	commandList->setGraphicsPipelineResources(constantBuffer->descs, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

	setPSConstantBuffer(constantBuffer->constantBuffer);
}

void GraphicsContext::setPSConstantBuffer(const ConstantBuffer* const constantBuffer)
{
	commandList->get()->SetGraphicsRootConstantBufferView(11, constantBuffer->getGPUAddress());
}

void GraphicsContext::setCSConstantBuffer(const IndexConstantBuffer* const constantBuffer)
{
	commandList->setComputePipelineResources(constantBuffer->descs);

	setCSConstantBuffer(constantBuffer->constantBuffer);
}

void GraphicsContext::setCSConstantBuffer(const ConstantBuffer* const constantBuffer)
{
	commandList->get()->SetComputeRootConstantBufferView(3, constantBuffer->getGPUAddress());
}


void GraphicsContext::transitionResources()
{
	commandList->transitionResources();
}

void GraphicsContext::setRenderTargets(const std::initializer_list<RenderTargetDesc>& renderTargets, const DepthStencilDesc* const depthStencils)
{
	commandList->setRenderTargets(renderTargets, depthStencils);
}

void GraphicsContext::setDefRenderTarget() const
{
	commandList->setDefRenderTarget();
}

void GraphicsContext::clearDefRenderTarget(const FLOAT clearValue[4]) const
{
	commandList->clearDefRenderTarget(clearValue);
}

void GraphicsContext::setVertexBuffers(const UINT startSlot, const std::initializer_list<VertexBufferDesc>& vertexBuffers)
{
	commandList->setVertexBuffers(startSlot, vertexBuffers);
}

void GraphicsContext::setIndexBuffer(const IndexBufferDesc& indexBuffer)
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

void GraphicsContext::setViewportSimple(const float width, const float height)
{
	setViewport(width, height);
	setScissorRect(0, 0, width, height);
}

void GraphicsContext::setViewportSimple(const UINT width, const UINT height)
{
	setViewport(width, height);
	setScissorRect(0, 0, width, height);
}

void GraphicsContext::setPipelineState(ID3D12PipelineState* const pipelineState) const
{
	commandList->get()->SetPipelineState(pipelineState);
}

void GraphicsContext::clearRenderTarget(const RenderTargetDesc& desc, const FLOAT clearValue[4]) const
{
	commandList->get()->ClearRenderTargetView(desc.rtvHandle, clearValue, 0, nullptr);
}

void GraphicsContext::clearDepthStencil(const DepthStencilDesc& desc, const D3D12_CLEAR_FLAGS flags, const FLOAT depth, const UINT8 stencil) const
{
	commandList->get()->ClearDepthStencilView(desc.dsvHandle, flags, depth, stencil, 0, nullptr);
}

void GraphicsContext::clearUnorderedAccess(const ClearUAVDesc& desc, const FLOAT values[4])
{
	commandList->clearUnorderedAccessView(desc, values);
}

void GraphicsContext::clearUnorderedAccess(const ClearUAVDesc& desc, const UINT values[4])
{
	commandList->clearUnorderedAccessView(desc, values);
}

void GraphicsContext::uavBarrier(const std::initializer_list<Resource*>& resources) const
{
	commandList->uavBarrier(resources);
}

void GraphicsContext::draw(const UINT vertexCountPerInstance, const UINT instanceCount, const UINT startVertexLocation, const UINT startInstanceLocation) const
{
	commandList->get()->DrawInstanced(vertexCountPerInstance, instanceCount, startVertexLocation, startInstanceLocation);
}

void GraphicsContext::drawIndexed(const UINT indexCountPerInstance, const UINT instanceCount, const UINT startIndexLocation, const INT baseVertexLocation, const UINT startInstanceLocation) const
{
	commandList->get()->DrawIndexedInstanced(indexCountPerInstance, instanceCount, startIndexLocation, baseVertexLocation, startInstanceLocation);
}

void GraphicsContext::dispatch(const UINT threadGroupCountX, const UINT threadGroupCountY, const UINT threadGroupCountZ) const
{
	commandList->get()->Dispatch(threadGroupCountX, threadGroupCountY, threadGroupCountZ);
}

void GraphicsContext::begin() const
{
	commandList->open();

	commandList->setDescriptorHeap(GlobalDescriptorHeap::getResourceHeap(), GlobalDescriptorHeap::getSamplerHeap());

	commandList->setGraphicsRootSignature(GlobalRootSignature::getGraphicsRootSignature());

	commandList->setComputeRootSignature(GlobalRootSignature::getComputeRootSignature());

	commandList->get()->SetGraphicsRootConstantBufferView(0, globalConstantBuffer->getGPUAddress());

	commandList->get()->SetComputeRootConstantBufferView(0, globalConstantBuffer->getGPUAddress());
}

void GraphicsContext::end() const
{
	commandList->close();
}

CommandList* GraphicsContext::getCommandList() const
{
	return commandList;
}

void GraphicsContext::getIndicesFromResourceDescs(const std::initializer_list<ShaderResourceDesc>& descs, UINT* const dst) const
{
	UINT index = 0;

	for (const ShaderResourceDesc& desc : descs)
	{
		dst[index] = desc.resourceIndex;

		index++;
	}
}