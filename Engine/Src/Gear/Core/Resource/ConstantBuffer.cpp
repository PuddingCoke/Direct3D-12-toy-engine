#include<Gear/Core/Resource/ConstantBuffer.h>

ConstantBufferPool* ConstantBuffer::bufferPools[3] = { nullptr,nullptr,nullptr };

ConstantBuffer::ConstantBuffer(const UINT size, const bool cpuWritable, const void* const data, ID3D12GraphicsCommandList6* commandList, std::vector<Resource*>* transientResourcePool) :
	size(size)
{
	if (cpuWritable)
	{
		buffer = nullptr;

		switch (size)
		{
		case 256:
			poolIndex = 0;
			break;
		case 512:
			poolIndex = 1;
			break;
		case 1024:
			poolIndex = 2;
			break;
		default:
			throw "size should be one of 256 512 1024";
			break;
		}

		ConstantBufferPool::AvailableDescriptor availableDescriptor = bufferPools[poolIndex]->requestAvailableDescriptor();

		gpuAddress = availableDescriptor.gpuAddress;

		bufferIndex = availableDescriptor.descriptorIndex;
	}
	else
	{
		if (data == nullptr)
		{
			throw "data should not be nullptr if cpuWritable is set to true";
		}

		buffer = new Buffer(size, false, data, commandList, transientResourcePool, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);

		poolIndex = 0;

		D3D12_CONSTANT_BUFFER_VIEW_DESC desc = {};
		desc.BufferLocation = buffer->getGPUAddress();
		desc.SizeInBytes = size;

		StaticDescriptorHandle descriptorHandle = GlobalDescriptorHeap::getResourceHeap()->allocStaticDescriptor(1);

		GraphicsDevice::get()->CreateConstantBufferView(&desc, descriptorHandle.getCPUHandle());

		gpuAddress = buffer->getGPUAddress();

		bufferIndex = descriptorHandle.getCurrentIndex();
	}
}

void ConstantBuffer::update(const void* const data, const UINT size)
{
	bufferPools[poolIndex]->updateSubRegion(bufferIndex, data, size);
}

ConstantBuffer::~ConstantBuffer()
{
	if (buffer)
	{
		delete buffer;
	}
	else
	{
		bufferPools[poolIndex]->retrieveUsedDescriptor(bufferIndex);
	}
}

ShaderResourceDesc ConstantBuffer::getBufferIndex() const
{
	ShaderResourceDesc desc = {};
	desc.type = ShaderResourceDesc::BUFFER;
	desc.state = ShaderResourceDesc::CBV;
	desc.bufferDesc.buffer = buffer;
	desc.bufferDesc.resourceIndex = bufferIndex;

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
