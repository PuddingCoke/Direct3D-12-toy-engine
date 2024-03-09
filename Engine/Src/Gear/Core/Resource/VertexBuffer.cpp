#include<Gear/Core/Resource/VertexBuffer.h>

VertexBuffer::VertexBuffer(const UINT perVertexSize, const UINT size, const bool cpuWritable, const void* const data, ID3D12GraphicsCommandList6* commandList, std::vector<Resource*>* transientResourcePool) :
	uploadHeaps(nullptr), uploadHeapIndex(0)
{
	buffer = new Buffer(size, cpuWritable, data, commandList, transientResourcePool, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);

	vertexBufferView.BufferLocation = buffer->getGPUAddress();
	vertexBufferView.SizeInBytes = size;
	vertexBufferView.StrideInBytes = perVertexSize;

	if (cpuWritable)
	{
		uploadHeaps = new UploadHeap * [Graphics::FrameBufferCount];

		for (UINT i = 0; i < Graphics::FrameBufferCount; i++)
		{
			uploadHeaps[i] = new UploadHeap(size);
		}
	}
}

VertexBuffer::VertexBuffer(const VertexBuffer& vb) :
	vertexBufferView(vb.vertexBufferView),
	uploadHeaps(vb.uploadHeaps),
	uploadHeapIndex(vb.uploadHeapIndex),
	buffer(new Buffer(*(vb.buffer)))
{
}

VertexBuffer::~VertexBuffer()
{
	if (buffer)
	{
		delete buffer;
	}

	if (uploadHeaps)
	{
		for (UINT i = 0; i < Graphics::FrameBufferCount; i++)
		{
			delete uploadHeaps[i];
		}

		delete[] uploadHeaps;
	}
}

D3D12_VERTEX_BUFFER_VIEW VertexBuffer::getVertexBufferView() const
{
	return vertexBufferView;
}

Buffer* VertexBuffer::getBuffer() const
{
	return buffer;
}
