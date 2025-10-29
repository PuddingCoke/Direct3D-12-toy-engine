#include<Gear/Core/Resource/ImmutableCBuffer.h>

Gear::Core::Resource::ImmutableCBuffer::ImmutableCBuffer(D3D12Resource::Buffer* const buffer, const uint32_t size, const bool persistent) :
	ResourceBase(persistent), gpuAddress(), bufferIndex(), buffer(buffer)
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

		D3D12Core::DescriptorHandle descriptorHandle;

		if (persistent)
		{
			descriptorHandle = Gear::Core::GlobalDescriptorHeap::getResourceHeap()->allocStaticDescriptor(numSRVUAVCBVDescriptors);
		}
		else
		{
			descriptorHandle = Gear::Core::GlobalDescriptorHeap::getNonShaderVisibleResourceHeap()->allocDynamicDescriptor(numSRVUAVCBVDescriptors);
		}

		srvUAVCBVHandleStart = descriptorHandle.getCPUHandle();

		Gear::Core::GraphicsDevice::get()->CreateConstantBufferView(&desc, descriptorHandle.getCPUHandle());

		gpuAddress = buffer->getGPUAddress();

		bufferIndex = descriptorHandle.getCurrentIndex();
	}
}

Gear::Core::Resource::ImmutableCBuffer::~ImmutableCBuffer()
{
	if (buffer)
	{
		delete buffer;
	}
}

Gear::Core::Resource::D3D12Resource::ShaderResourceDesc Gear::Core::Resource::ImmutableCBuffer::getBufferIndex() const
{
	D3D12Resource::ShaderResourceDesc desc = {};
	desc.type = D3D12Resource::ShaderResourceDesc::BUFFER;
	desc.state = D3D12Resource::ShaderResourceDesc::CBV;
	desc.resourceIndex = bufferIndex;
	desc.bufferDesc.buffer = buffer;

	return desc;
}

D3D12_GPU_VIRTUAL_ADDRESS Gear::Core::Resource::ImmutableCBuffer::getGPUAddress() const
{
	return gpuAddress;
}

Gear::Core::Resource::D3D12Resource::Buffer* Gear::Core::Resource::ImmutableCBuffer::getBuffer() const
{
	return buffer;
}

void Gear::Core::Resource::ImmutableCBuffer::copyDescriptors()
{
	const D3D12Core::DescriptorHandle handle = getTransientDescriptorHandle();

	bufferIndex = handle.getCurrentIndex();
}
