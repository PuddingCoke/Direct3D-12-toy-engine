﻿#include<Gear/Core/GraphicsContext.h>

#include<Gear/Core/Graphics.h>

#include<Gear/Core/GlobalRootSignature.h>

Gear::Core::GraphicsContext::GraphicsContext() :
	commandList(new D3D12Core::CommandList(D3D12_COMMAND_LIST_TYPE_DIRECT)),
	vp{ 0.f,0.f,0.f,0.f,0.f,1.f },
	rt{ 0,0,0,0 },
	resourceIndices{}
{
}

Gear::Core::GraphicsContext::~GraphicsContext()
{
	if (commandList)
	{
		delete commandList;
	}
}

void Gear::Core::GraphicsContext::updateBuffer(Resource::BufferView* const bufferView, const void* const data, const uint32_t size) const
{
	const Resource::BufferView::UpdateStruct updateStruct = bufferView->getUpdateStruct(data, size);

	commandList->copyBufferRegion(updateStruct.buffer, 0, updateStruct.uploadHeap, 0, size);
}

void Gear::Core::GraphicsContext::updateBuffer(Resource::StaticCBuffer* const staticCBuffer, const void* const data, const uint32_t size) const
{
	const Resource::StaticCBuffer::UpdateStruct updateStruct = staticCBuffer->getUpdateStruct(data, size);

	commandList->copyBufferRegion(updateStruct.buffer, 0, updateStruct.uploadHeap, 0, size);
}

void Gear::Core::GraphicsContext::setVSConstants(const std::initializer_list<Resource::D3D12Resource::ShaderResourceDesc>& descs, const uint32_t offset)
{
	getResourceIndicesFromDescs(descs);

	commandList->setGraphicsPipelineResources(descs, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);

	setVSConstants(static_cast<uint32_t>(descs.size()), resourceIndices, offset);
}

void Gear::Core::GraphicsContext::setVSConstants(const uint32_t numValues, const void* const data, const uint32_t offset) const
{
	commandList->setGraphicsRootConstants(2, numValues, data, offset);
}

void Gear::Core::GraphicsContext::setHSConstants(const std::initializer_list<Resource::D3D12Resource::ShaderResourceDesc>& descs, const uint32_t offset)
{
	getResourceIndicesFromDescs(descs);

	commandList->setGraphicsPipelineResources(descs, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);

	setHSConstants(static_cast<uint32_t>(descs.size()), resourceIndices, offset);
}

void Gear::Core::GraphicsContext::setHSConstants(const uint32_t numValues, const void* const data, const uint32_t offset) const
{
	commandList->setGraphicsRootConstants(3, numValues, data, offset);
}

void Gear::Core::GraphicsContext::setDSConstants(const std::initializer_list<Resource::D3D12Resource::ShaderResourceDesc>& descs, const uint32_t offset)
{
	getResourceIndicesFromDescs(descs);

	commandList->setGraphicsPipelineResources(descs, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);

	setDSConstants(static_cast<uint32_t>(descs.size()), resourceIndices, offset);
}

void Gear::Core::GraphicsContext::setDSConstants(const uint32_t numValues, const void* const data, const uint32_t offset) const
{
	commandList->setGraphicsRootConstants(4, numValues, data, offset);
}

void Gear::Core::GraphicsContext::setGSConstants(const std::initializer_list<Resource::D3D12Resource::ShaderResourceDesc>& descs, const uint32_t offset)
{
	getResourceIndicesFromDescs(descs);

	commandList->setGraphicsPipelineResources(descs, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);

	setGSConstants(static_cast<uint32_t>(descs.size()), resourceIndices, offset);
}

void Gear::Core::GraphicsContext::setGSConstants(const uint32_t numValues, const void* const data, const uint32_t offset) const
{
	commandList->setGraphicsRootConstants(5, numValues, data, offset);
}

void Gear::Core::GraphicsContext::setPSConstants(const std::initializer_list<Resource::D3D12Resource::ShaderResourceDesc>& descs, const uint32_t offset)
{
	getResourceIndicesFromDescs(descs);

	commandList->setGraphicsPipelineResources(descs, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

	setPSConstants(static_cast<uint32_t>(descs.size()), resourceIndices, offset);
}

void Gear::Core::GraphicsContext::setPSConstants(const uint32_t numValues, const void* const data, const uint32_t offset) const
{
	commandList->setGraphicsRootConstants(6, numValues, data, offset);
}

void Gear::Core::GraphicsContext::setCSConstants(const std::initializer_list<Resource::D3D12Resource::ShaderResourceDesc>& descs, const uint32_t offset)
{
	getResourceIndicesFromDescs(descs);

	commandList->setComputePipelineResources(descs);

	setCSConstants(static_cast<uint32_t>(descs.size()), resourceIndices, offset);
}

void Gear::Core::GraphicsContext::setCSConstants(const uint32_t numValues, const void* const data, const uint32_t offset) const
{
	commandList->setComputeRootConstants(2, numValues, data, offset);
}

void Gear::Core::GraphicsContext::setGlobalConstantBuffer(const Resource::ImmutableCBuffer* const immutableCBuffer)
{
	Resource::D3D12Resource::Buffer* const buffer = immutableCBuffer->getBuffer();

	if (buffer && buffer->getStateTracking())
	{
		commandList->pushResourceTrackList(buffer);

		buffer->setState(D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
	}

	pushRootConstantBufferDesc({ RootConstantBufferDesc::GRAPHICS,1,immutableCBuffer->getGPUAddress() });

	pushRootConstantBufferDesc({ RootConstantBufferDesc::COMPUTE,1,immutableCBuffer->getGPUAddress() });
}

void Gear::Core::GraphicsContext::setVSConstantBuffer(const Resource::ImmutableCBuffer* const immutableCBuffer)
{
	Resource::D3D12Resource::Buffer* const buffer = immutableCBuffer->getBuffer();

	if (buffer && buffer->getStateTracking())
	{
		commandList->pushResourceTrackList(buffer);

		buffer->setState(D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
	}

	pushRootConstantBufferDesc({ RootConstantBufferDesc::GRAPHICS,7,immutableCBuffer->getGPUAddress() });
}

void Gear::Core::GraphicsContext::setHSConstantBuffer(const Resource::ImmutableCBuffer* const immutableCBuffer)
{
	Resource::D3D12Resource::Buffer* const buffer = immutableCBuffer->getBuffer();

	if (buffer && buffer->getStateTracking())
	{
		commandList->pushResourceTrackList(buffer);

		buffer->setState(D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
	}

	pushRootConstantBufferDesc({ RootConstantBufferDesc::GRAPHICS,8,immutableCBuffer->getGPUAddress() });
}

void Gear::Core::GraphicsContext::setDSConstantBuffer(const Resource::ImmutableCBuffer* const immutableCBuffer)
{
	Resource::D3D12Resource::Buffer* const buffer = immutableCBuffer->getBuffer();

	if (buffer && buffer->getStateTracking())
	{
		commandList->pushResourceTrackList(buffer);

		buffer->setState(D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
	}

	pushRootConstantBufferDesc({ RootConstantBufferDesc::GRAPHICS,9,immutableCBuffer->getGPUAddress() });
}

void Gear::Core::GraphicsContext::setGSConstantBuffer(const Resource::ImmutableCBuffer* const immutableCBuffer)
{
	Resource::D3D12Resource::Buffer* const buffer = immutableCBuffer->getBuffer();

	if (buffer && buffer->getStateTracking())
	{
		commandList->pushResourceTrackList(buffer);

		buffer->setState(D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
	}

	pushRootConstantBufferDesc({ RootConstantBufferDesc::GRAPHICS,10,immutableCBuffer->getGPUAddress() });
}

void Gear::Core::GraphicsContext::setPSConstantBuffer(const Resource::ImmutableCBuffer* const immutableCBuffer)
{
	Resource::D3D12Resource::Buffer* const buffer = immutableCBuffer->getBuffer();

	if (buffer && buffer->getStateTracking())
	{
		commandList->pushResourceTrackList(buffer);

		buffer->setState(D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
	}

	pushRootConstantBufferDesc({ RootConstantBufferDesc::GRAPHICS,11,immutableCBuffer->getGPUAddress() });
}

void Gear::Core::GraphicsContext::setCSConstantBuffer(const Resource::ImmutableCBuffer* const immutableCBuffer)
{
	Resource::D3D12Resource::Buffer* const buffer = immutableCBuffer->getBuffer();

	if (buffer && buffer->getStateTracking())
	{
		commandList->pushResourceTrackList(buffer);

		buffer->setState(D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
	}

	pushRootConstantBufferDesc({ RootConstantBufferDesc::COMPUTE,3,immutableCBuffer->getGPUAddress() });
}

void Gear::Core::GraphicsContext::transitionResources()
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

				commandList->setGraphicsRootConstantBuffer(rootParameterIndex, gpuAddress);
			}
			else
			{
				const uint32_t rootParameterIndex = rootConstantBufferDescs[i].rootParameterIndex;

				const D3D12_GPU_VIRTUAL_ADDRESS gpuAddress = rootConstantBufferDescs[i].gpuAddress;

				commandList->setComputeRootConstantBuffer(rootParameterIndex, gpuAddress);
			}
		}

		rootConstantBufferDescs.clear();
	}
}

void Gear::Core::GraphicsContext::setRenderTargets(const std::initializer_list<Resource::D3D12Resource::RenderTargetDesc>& renderTargets, const Resource::D3D12Resource::DepthStencilDesc* const depthStencils) const
{
	commandList->setRenderTargets(renderTargets, depthStencils);
}

void Gear::Core::GraphicsContext::setDefRenderTarget() const
{
	commandList->setDefRenderTarget();
}

void Gear::Core::GraphicsContext::clearDefRenderTarget(const float clearValue[4]) const
{
	commandList->clearDefRenderTarget(clearValue);
}

void Gear::Core::GraphicsContext::setVertexBuffers(const uint32_t startSlot, const std::initializer_list<Resource::D3D12Resource::VertexBufferDesc>& vertexBuffers) const
{
	commandList->setVertexBuffers(startSlot, vertexBuffers);
}

void Gear::Core::GraphicsContext::setIndexBuffer(const Resource::D3D12Resource::IndexBufferDesc& indexBuffer) const
{
	commandList->setIndexBuffer(indexBuffer);
}

void Gear::Core::GraphicsContext::setPrimitiveTopology(const D3D12_PRIMITIVE_TOPOLOGY topology) const
{
	commandList->setPrimitiveTopology(topology);
}

void Gear::Core::GraphicsContext::setViewport(const float width, const float height)
{
	vp.Width = width;
	vp.Height = height;

	commandList->setViewports(1, &vp);
}

void Gear::Core::GraphicsContext::setViewport(const uint32_t width, const uint32_t height)
{
	setViewport(static_cast<float>(width), static_cast<float>(height));
}

void Gear::Core::GraphicsContext::setScissorRect(const uint32_t left, const uint32_t top, const uint32_t right, const uint32_t bottom)
{
	rt.left = left;
	rt.top = top;
	rt.right = right;
	rt.bottom = bottom;

	commandList->setScissorRects(1, &rt);
}

void Gear::Core::GraphicsContext::setScissorRect(const float left, const float top, const float right, const float bottom)
{
	setScissorRect(static_cast<uint32_t>(left), static_cast<uint32_t>(top), static_cast<uint32_t>(right), static_cast<uint32_t>(bottom));
}

void Gear::Core::GraphicsContext::setViewportSimple(const float width, const float height)
{
	setViewport(width, height);
	setScissorRect(0, 0, width, height);
}

void Gear::Core::GraphicsContext::setViewportSimple(const uint32_t width, const uint32_t height)
{
	setViewport(width, height);
	setScissorRect(0, 0, width, height);
}

void Gear::Core::GraphicsContext::setPipelineState(ID3D12PipelineState* const pipelineState) const
{
	commandList->setPipelineState(pipelineState);
}

void Gear::Core::GraphicsContext::clearRenderTarget(const Resource::D3D12Resource::RenderTargetDesc& desc, const float clearValue[4]) const
{
	commandList->clearRenderTargetView(desc.rtvHandle, clearValue, 0, nullptr);
}

void Gear::Core::GraphicsContext::clearDepthStencil(const Resource::D3D12Resource::DepthStencilDesc& desc, const D3D12_CLEAR_FLAGS flags, const float depth, const uint8_t stencil) const
{
	commandList->clearDepthStencilView(desc.dsvHandle, flags, depth, stencil, 0, nullptr);
}

void Gear::Core::GraphicsContext::clearUnorderedAccess(const Resource::D3D12Resource::ClearUAVDesc& desc, const float values[4])
{
	commandList->clearUnorderedAccessView(desc, values);
}

void Gear::Core::GraphicsContext::clearUnorderedAccess(const Resource::D3D12Resource::ClearUAVDesc& desc, const uint32_t values[4])
{
	commandList->clearUnorderedAccessView(desc, values);
}

void Gear::Core::GraphicsContext::uavBarrier(const std::initializer_list<Resource::D3D12Resource::D3D12ResourceBase*>& resources) const
{
	commandList->uavBarrier(resources);
}

void Gear::Core::GraphicsContext::draw(const uint32_t vertexCountPerInstance, const uint32_t instanceCount, const uint32_t startVertexLocation, const uint32_t startInstanceLocation) const
{
	commandList->drawInstanced(vertexCountPerInstance, instanceCount, startVertexLocation, startInstanceLocation);
}

void Gear::Core::GraphicsContext::drawIndexed(const uint32_t indexCountPerInstance, const uint32_t instanceCount, const uint32_t startIndexLocation, const int32_t baseVertexLocation, const uint32_t startInstanceLocation) const
{
	commandList->drawIndexedInstanced(indexCountPerInstance, instanceCount, startIndexLocation, baseVertexLocation, startInstanceLocation);
}

void Gear::Core::GraphicsContext::dispatch(const uint32_t threadGroupCountX, const uint32_t threadGroupCountY, const uint32_t threadGroupCountZ) const
{
	commandList->dispatch(threadGroupCountX, threadGroupCountY, threadGroupCountZ);
}

void Gear::Core::GraphicsContext::begin() const
{
	commandList->open();

	commandList->setDescriptorHeap(GlobalDescriptorHeap::getResourceHeap(), GlobalDescriptorHeap::getSamplerHeap());

	commandList->setGraphicsRootSignature(GlobalRootSignature::getGraphicsRootSignature());

	commandList->setComputeRootSignature(GlobalRootSignature::getComputeRootSignature());

	commandList->setGraphicsRootConstantBuffer(0, Graphics::getReservedGlobalCBuffer()->getGPUAddress());

	commandList->setComputeRootConstantBuffer(0, Graphics::getReservedGlobalCBuffer()->getGPUAddress());
}

Gear::Core::D3D12Core::CommandList* Gear::Core::GraphicsContext::getCommandList() const
{
	return commandList;
}

void Gear::Core::GraphicsContext::getResourceIndicesFromDescs(const std::initializer_list<Resource::D3D12Resource::ShaderResourceDesc>& descs)
{
	std::transform(descs.begin(), descs.end(), resourceIndices,
		[](const Resource::D3D12Resource::ShaderResourceDesc& desc)
		{
			return desc.resourceIndex;
		});
}

void Gear::Core::GraphicsContext::pushRootConstantBufferDesc(const RootConstantBufferDesc& desc)
{
	rootConstantBufferDescs.emplace_back(desc);
}
