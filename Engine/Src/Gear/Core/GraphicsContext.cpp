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

	commandList->pushResourceTrackList(buffer);

	buffer->setState(D3D12_RESOURCE_STATE_COPY_DEST);

	transitionResources();

	UploadHeap* const uploadHeap = vb->uploadHeaps[vb->uploadHeapIndex];

	uploadHeap->update(data, size);

	commandList->get()->CopyBufferRegion(buffer->getResource(), 0, uploadHeap->getResource(), 0, size);

	vb->uploadHeapIndex = (vb->uploadHeapIndex + 1) % Graphics::FrameBufferCount;
}

void GraphicsContext::updateBuffer(IndexBuffer* const ib, const void* const data, const UINT size)
{
	Buffer* const buffer = ib->getBuffer();

	commandList->pushResourceTrackList(buffer);

	buffer->setState(D3D12_RESOURCE_STATE_COPY_DEST);

	transitionResources();

	UploadHeap* const uploadHeap = ib->uploadHeaps[ib->uploadHeapIndex];

	uploadHeap->update(data, size);

	commandList->get()->CopyBufferRegion(buffer->getResource(), 0, uploadHeap->getResource(), 0, size);

	ib->uploadHeapIndex = (ib->uploadHeapIndex + 1) % Graphics::FrameBufferCount;
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
	UINT indices[4] = {};

	getIndicesFromResourceDescs(descs, indices);

	commandList->setGraphicsPipelineResources(descs, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);

	setVSConstants(descs.size(), indices, offset);
}

void GraphicsContext::setVSConstants(const UINT numValues, const void* const data, const UINT offset)
{
	commandList->get()->SetGraphicsRoot32BitConstants(2, numValues, data, offset);
}

void GraphicsContext::setHSConstants(const std::initializer_list<ShaderResourceDesc>& descs, const UINT offset)
{
	UINT indices[4] = {};

	getIndicesFromResourceDescs(descs, indices);

	commandList->setGraphicsPipelineResources(descs, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);

	setHSConstants(descs.size(), indices, offset);
}

void GraphicsContext::setHSConstants(const UINT numValues, const void* const data, const UINT offset)
{
	commandList->get()->SetGraphicsRoot32BitConstants(3, numValues, data, offset);
}

void GraphicsContext::setDSConstants(const std::initializer_list<ShaderResourceDesc>& descs, const UINT offset)
{
	UINT indices[4] = {};

	getIndicesFromResourceDescs(descs, indices);

	commandList->setGraphicsPipelineResources(descs, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);

	setDSConstants(descs.size(), indices, offset);
}

void GraphicsContext::setDSConstants(const UINT numValues, const void* const data, const UINT offset)
{
	commandList->get()->SetGraphicsRoot32BitConstants(4, numValues, data, offset);
}

void GraphicsContext::setGSConstants(const std::initializer_list<ShaderResourceDesc>& descs, const UINT offset)
{
	UINT indices[4] = {};

	getIndicesFromResourceDescs(descs, indices);

	commandList->setGraphicsPipelineResources(descs, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);

	setGSConstants(descs.size(), indices, offset);
}

void GraphicsContext::setGSConstants(const UINT numValues, const void* const data, const UINT offset)
{
	commandList->get()->SetGraphicsRoot32BitConstants(5, numValues, data, offset);
}

void GraphicsContext::setPSConstants(const std::initializer_list<ShaderResourceDesc>& descs, const UINT offset)
{
	UINT indices[16] = {};

	getIndicesFromResourceDescs(descs, indices);

	commandList->setGraphicsPipelineResources(descs, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

	setPSConstants(descs.size(), indices, offset);
}

void GraphicsContext::setPSConstants(const UINT numValues, const void* const data, const UINT offset)
{
	commandList->get()->SetGraphicsRoot32BitConstants(6, numValues, data, offset);
}

void GraphicsContext::setASConstants(const std::initializer_list<ShaderResourceDesc>& descs, const UINT offset)
{
	UINT indices[4] = {};

	getIndicesFromResourceDescs(descs, indices);

	commandList->setGraphicsPipelineResources(descs, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);

	setASConstants(descs.size(), indices, offset);
}

void GraphicsContext::setASConstants(const UINT numValues, const void* const data, const UINT offset)
{
	commandList->get()->SetGraphicsRoot32BitConstants(7, numValues, data, offset);
}

void GraphicsContext::setMSConstants(const std::initializer_list<ShaderResourceDesc>& descs, const UINT offset)
{
	UINT indices[4] = {};

	getIndicesFromResourceDescs(descs, indices);

	commandList->setGraphicsPipelineResources(descs, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);

	setMSConstants(descs.size(), indices, offset);
}

void GraphicsContext::setMSConstants(const UINT numValues, const void* const data, const UINT offset)
{
	commandList->get()->SetGraphicsRoot32BitConstants(8, numValues, data, offset);
}

void GraphicsContext::setCSConstants(const std::initializer_list<ShaderResourceDesc>& descs, const UINT offset)
{
	UINT indices[32] = {};

	getIndicesFromResourceDescs(descs, indices);

	commandList->setComputePipelineResources(descs);

	setCSConstants(descs.size(), indices, offset);
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
	commandList->get()->SetGraphicsRootConstantBufferView(9, constantBuffer->getGPUAddress());
}

void GraphicsContext::setHSConstantBuffer(const IndexConstantBuffer* const constantBuffer)
{
	commandList->setGraphicsPipelineResources(constantBuffer->descs, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);

	setHSConstantBuffer(constantBuffer->constantBuffer);
}

void GraphicsContext::setHSConstantBuffer(const ConstantBuffer* const constantBuffer)
{
	commandList->get()->SetGraphicsRootConstantBufferView(10, constantBuffer->getGPUAddress());
}

void GraphicsContext::setDSConstantBuffer(const IndexConstantBuffer* const constantBuffer)
{
	commandList->setGraphicsPipelineResources(constantBuffer->descs, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);

	setDSConstantBuffer(constantBuffer->constantBuffer);
}

void GraphicsContext::setDSConstantBuffer(const ConstantBuffer* const constantBuffer)
{
	commandList->get()->SetGraphicsRootConstantBufferView(11, constantBuffer->getGPUAddress());
}

void GraphicsContext::setGSConstantBuffer(const IndexConstantBuffer* const constantBuffer)
{
	commandList->setGraphicsPipelineResources(constantBuffer->descs, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);

	setGSConstantBuffer(constantBuffer->constantBuffer);
}

void GraphicsContext::setGSConstantBuffer(const ConstantBuffer* const constantBuffer)
{
	commandList->get()->SetGraphicsRootConstantBufferView(12, constantBuffer->getGPUAddress());
}

void GraphicsContext::setPSConstantBuffer(const IndexConstantBuffer* const constantBuffer)
{
	commandList->setGraphicsPipelineResources(constantBuffer->descs, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

	setPSConstantBuffer(constantBuffer->constantBuffer);
}

void GraphicsContext::setPSConstantBuffer(const ConstantBuffer* const constantBuffer)
{
	commandList->get()->SetGraphicsRootConstantBufferView(13, constantBuffer->getGPUAddress());
}

void GraphicsContext::setASConstantBuffer(const IndexConstantBuffer* const constantBuffer)
{
	commandList->setGraphicsPipelineResources(constantBuffer->descs, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);

	setASConstantBuffer(constantBuffer->constantBuffer);
}

void GraphicsContext::setASConstantBuffer(const ConstantBuffer* const constantBuffer)
{
	commandList->get()->SetGraphicsRootConstantBufferView(14, constantBuffer->getGPUAddress());
}

void GraphicsContext::setMSConstantBuffer(const IndexConstantBuffer* const constantBuffer)
{
	commandList->setGraphicsPipelineResources(constantBuffer->descs, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);

	setMSConstantBuffer(constantBuffer->constantBuffer);
}

void GraphicsContext::setMSConstantBuffer(const ConstantBuffer* const constantBuffer)
{
	commandList->get()->SetGraphicsRootConstantBufferView(15, constantBuffer->getGPUAddress());
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

void GraphicsContext::setRenderTargets(const std::initializer_list<RenderTargetDesc>& renderTargets, const std::initializer_list<DepthStencilDesc>& depthStencils)
{
	commandList->setRenderTargets(renderTargets, depthStencils);
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

		commandList->pushResourceTrackList(buffer);

		buffer->setState(D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);

		vbvs.push_back(vb->getVertexBufferView());
	}

	commandList->get()->IASetVertexBuffers(startSlot, vbvs.size(), vbvs.data());
}

void GraphicsContext::setIndexBuffers(const std::initializer_list<IndexBuffer*>& indexBuffers)
{
	std::vector<D3D12_INDEX_BUFFER_VIEW> ibvs;

	for (IndexBuffer* const ib : indexBuffers)
	{
		Buffer* buffer = ib->getBuffer();

		commandList->pushResourceTrackList(buffer);

		buffer->setState(D3D12_RESOURCE_STATE_INDEX_BUFFER);

		ibvs.push_back(ib->getIndexBufferView());
	}

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

void GraphicsContext::clearRenderTarget(const RenderTargetDesc desc, const FLOAT clearValue[4]) const
{
	commandList->get()->ClearRenderTargetView(desc.rtvHandle, clearValue, 0, nullptr);
}

void GraphicsContext::clearDepthStencil(const DepthStencilDesc desc, const D3D12_CLEAR_FLAGS flags, const FLOAT depth, const UINT8 stencil) const
{
	commandList->get()->ClearDepthStencilView(desc.dsvHandle, flags, depth, stencil, 0, nullptr);
}

void GraphicsContext::uavBarrier(const std::initializer_list<Resource*>& resources) const
{
	commandList->uavBarrier(resources);
}

void GraphicsContext::draw(const UINT vertexCountPerInstance, const UINT instanceCount, const UINT startVertexLocation, const UINT startInstanceLocation) const
{
	commandList->get()->DrawInstanced(vertexCountPerInstance, instanceCount, startVertexLocation, startInstanceLocation);
}

void GraphicsContext::dispatch(const UINT threadGroupCountX, const UINT threadGroupCountY, const UINT threadGroupCountZ)
{
	commandList->get()->Dispatch(threadGroupCountX, threadGroupCountY, threadGroupCountZ);
}

void GraphicsContext::begin() const
{
	commandList->reset();

	commandList->setDescriptorHeap(GlobalDescriptorHeap::getResourceHeap(), GlobalDescriptorHeap::getSamplerHeap());

	commandList->setGraphicsRootSignature(GlobalRootSignature::getGraphicsRootSignature());

	commandList->setComputeRootSignature(GlobalRootSignature::getComputeRootSignature());

	commandList->get()->SetGraphicsRootConstantBufferView(0, globalConstantBuffer->getGPUAddress());

	commandList->get()->SetComputeRootConstantBufferView(0, globalConstantBuffer->getGPUAddress());
}

void GraphicsContext::end() const
{
	commandList->get()->Close();
}

CommandList* GraphicsContext::getCommandList() const
{
	return commandList;
}

void GraphicsContext::getIndicesFromResourceDescs(const std::initializer_list<ShaderResourceDesc>& descs, UINT* const dst)
{
	UINT index = 0;

	for (const ShaderResourceDesc& desc : descs)
	{
		if (desc.type == ShaderResourceDesc::BUFFER)
		{
			dst[index] = desc.bufferDesc.resourceIndex;
		}
		else
		{
			dst[index] = desc.textureDesc.resourceIndex;
		}

		index++;
	}
}