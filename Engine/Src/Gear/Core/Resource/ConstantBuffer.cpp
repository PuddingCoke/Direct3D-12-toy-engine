#include<Gear/Core/Resource/ConstantBuffer.h>

ConstantBuffer::ConstantBuffer(const UINT size, const bool stateTracking, const bool cpuWritable, const void* const data, ID3D12GraphicsCommandList6* commandList, std::vector<Resource*>& transientResourcePool)
{
	buffer = new Buffer(size, stateTracking, cpuWritable, data, commandList, transientResourcePool);

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

TransitionDesc ConstantBuffer::getBufferIndex() const
{
	TransitionDesc desc = {};
	desc.type = TransitionDesc::BUFFER;
	desc.state = TransitionDesc::CBV;
	desc.buffer.buffer = buffer;
	desc.buffer.resourceIndex = bufferIndex;

	return desc;
}

Buffer* ConstantBuffer::getBuffer() const
{
	return buffer;
}
