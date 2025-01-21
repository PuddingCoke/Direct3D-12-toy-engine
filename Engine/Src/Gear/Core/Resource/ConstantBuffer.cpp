#include<Gear/Core/Resource/ConstantBuffer.h>

ConstantBuffer::ConstantBuffer(Buffer* const buffer, const uint32_t size, const bool persistent) :
	EngineResource(persistent), buffer(buffer)
{
	if (size % 256 != 0)
	{
		LOGERROR("size of constant buffer must be multiply of 256");
	}

	if (buffer == nullptr)
	{
		switch (size)
		{
		case 256:
			regionIndex = 0;
			break;
		case 512:
			regionIndex = 1;
			break;
		case 1024:
			regionIndex = 2;
			break;
		default:
			LOGERROR("size should be one of 256 512 1024");
			break;
		}

		ConstantBufferManager::AvailableDescriptor availableDescriptor = ConstantBufferManager::get()->requestDescriptor(regionIndex);

		gpuAddress = availableDescriptor.gpuAddress;

		bufferIndex = availableDescriptor.descriptorIndex;
	}
	else
	{
		regionIndex = UINT_MAX;

		D3D12_CONSTANT_BUFFER_VIEW_DESC desc = {};
		desc.BufferLocation = buffer->getGPUAddress();
		desc.SizeInBytes = size;

		numSRVUAVCBVDescriptors = 1;

		DescriptorHandle descriptorHandle = DescriptorHandle();

		if (persistent)
		{
			descriptorHandle = GlobalDescriptorHeap::getResourceHeap()->allocStaticDescriptor(numSRVUAVCBVDescriptors);
		}
		else
		{
			descriptorHandle = GlobalDescriptorHeap::getNonShaderVisibleResourceHeap()->allocDynamicDescriptor(numSRVUAVCBVDescriptors);
		}

		srvUAVCBVHandleStart = descriptorHandle.getCPUHandle();

		GraphicsDevice::get()->CreateConstantBufferView(&desc, descriptorHandle.getCPUHandle());

		gpuAddress = buffer->getGPUAddress();

		bufferIndex = descriptorHandle.getCurrentIndex();
	}
}

void ConstantBuffer::update(const void* const data, const uint32_t size) const
{
	ConstantBufferManager::get()->updateSubregion(regionIndex, bufferIndex, data, size);
}

ConstantBuffer::~ConstantBuffer()
{
	if (buffer)
	{
		delete buffer;
	}
	else
	{
		ConstantBufferManager::get()->retrieveDescriptor(regionIndex, bufferIndex);
	}
}

ShaderResourceDesc ConstantBuffer::getBufferIndex() const
{
	ShaderResourceDesc desc = {};
	desc.type = ShaderResourceDesc::BUFFER;
	desc.state = ShaderResourceDesc::CBV;
	desc.resourceIndex = bufferIndex;
	desc.bufferDesc.buffer = buffer;

	return desc;
}

D3D12_GPU_VIRTUAL_ADDRESS ConstantBuffer::getGPUAddress() const
{
	return gpuAddress;
}

Buffer* ConstantBuffer::getBuffer() const
{
	return buffer;
}

void ConstantBuffer::copyDescriptors()
{
	const DescriptorHandle handle = GlobalDescriptorHeap::getResourceHeap()->allocDynamicDescriptor(numSRVUAVCBVDescriptors);

	GraphicsDevice::get()->CopyDescriptorsSimple(numSRVUAVCBVDescriptors, handle.getCPUHandle(), srvUAVCBVHandleStart, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	bufferIndex = handle.getCurrentIndex();
}
