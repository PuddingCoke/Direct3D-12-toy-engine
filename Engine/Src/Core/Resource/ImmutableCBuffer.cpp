#include<Gear/Core/Resource/ImmutableCBuffer.h>

ImmutableCBuffer::ImmutableCBuffer(Buffer* const buffer, const uint32_t size, const bool persistent) :
	buffer(buffer), EngineResource(persistent)
{
	if (size % 256 != 0)
	{
		LOGERROR(L"size of constant buffer must be multiply of 256");
	}

	if (buffer)
	{
		D3D12_CONSTANT_BUFFER_VIEW_DESC desc = {};
		desc.BufferLocation = buffer->getGPUAddress();
		desc.SizeInBytes = size;

		numSRVUAVCBVDescriptors = 1;

		DescriptorHandle descriptorHandle = DescriptorHandle();

		if (persistent)
		{
			descriptorHandle = Core::GlobalDescriptorHeap::getResourceHeap()->allocStaticDescriptor(numSRVUAVCBVDescriptors);
		}
		else
		{
			descriptorHandle = Core::GlobalDescriptorHeap::getNonShaderVisibleResourceHeap()->allocDynamicDescriptor(numSRVUAVCBVDescriptors);
		}

		srvUAVCBVHandleStart = descriptorHandle.getCPUHandle();

		Core::GraphicsDevice::get()->CreateConstantBufferView(&desc, descriptorHandle.getCPUHandle());

		gpuAddress = buffer->getGPUAddress();

		bufferIndex = descriptorHandle.getCurrentIndex();
	}
}

ImmutableCBuffer::~ImmutableCBuffer()
{
	if (buffer)
	{
		delete buffer;
	}
}

ShaderResourceDesc ImmutableCBuffer::getBufferIndex() const
{
	ShaderResourceDesc desc = {};
	desc.type = ShaderResourceDesc::BUFFER;
	desc.state = ShaderResourceDesc::CBV;
	desc.resourceIndex = bufferIndex;
	desc.bufferDesc.buffer = buffer;

	return desc;
}

D3D12_GPU_VIRTUAL_ADDRESS ImmutableCBuffer::getGPUAddress() const
{
	return gpuAddress;
}

Buffer* ImmutableCBuffer::getBuffer() const
{
	return buffer;
}

void ImmutableCBuffer::copyDescriptors()
{
	const DescriptorHandle handle = getTransientDescriptorHandle();

	bufferIndex = handle.getCurrentIndex();
}
