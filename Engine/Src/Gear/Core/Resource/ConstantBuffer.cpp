#include<Gear/Core/Resource/ConstantBuffer.h>

ConstantBuffer::ConstantBuffer(const UINT size, const bool stateTracking, const bool cpuWritable, const void* const data, ID3D12GraphicsCommandList6* commandList, std::vector<Resource*>& transientResourcePool)
{
	buffer = new Buffer(size, stateTracking, cpuWritable, data, commandList, transientResourcePool, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);

	D3D12_CONSTANT_BUFFER_VIEW_DESC desc = {};
	desc.BufferLocation = buffer->getGPUAddress();
	desc.SizeInBytes = size;

	StaticDescriptorHandle descriptorHandle = GlobalDescriptorHeap::getResourceHeap()->allocStaticDescriptor(1);

	GraphicsDevice::get()->CreateConstantBufferView(&desc, descriptorHandle.getCPUHandle());

	bufferIndex = descriptorHandle.getCurrentIndex();
}

ConstantBuffer::ConstantBuffer(const ConstantBuffer& cb) :
	bufferIndex(cb.bufferIndex),
	buffer(new Buffer(*(cb.buffer)))
{
}

ConstantBuffer::~ConstantBuffer()
{
	if (buffer)
	{
		delete buffer;
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

Buffer* ConstantBuffer::getBuffer() const
{
	return buffer;
}
