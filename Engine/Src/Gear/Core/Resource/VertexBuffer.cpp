#include<Gear/Core/Resource/VertexBuffer.h>

VertexBuffer::VertexBuffer(const UINT perVertexSize, const UINT size, const bool stateTracking, const bool cpuWritable, const void* const data, ID3D12GraphicsCommandList6* commandList, std::vector<Resource*>* transientResourcePool)
{
	buffer = new Buffer(size, stateTracking, data, commandList, transientResourcePool, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);

	vertexBufferView.BufferLocation = buffer->getGPUAddress();
	vertexBufferView.SizeInBytes = size;
	vertexBufferView.StrideInBytes = perVertexSize;
}

VertexBuffer::VertexBuffer(const VertexBuffer& vb) :
	vertexBufferView(vb.vertexBufferView),
	buffer(new Buffer(*(vb.buffer)))
{
}

VertexBuffer::~VertexBuffer()
{
}

D3D12_VERTEX_BUFFER_VIEW VertexBuffer::getVertexBufferView() const
{
	return vertexBufferView;
}

Buffer* VertexBuffer::getBuffer()
{
	return buffer;
}
