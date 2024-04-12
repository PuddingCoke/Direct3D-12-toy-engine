#include<Gear/Core/Resource/VertexBuffer.h>

VertexBuffer::VertexBuffer(Buffer* const buffer, const UINT perVertexSize, const UINT size, const bool cpuWritable) :
	buffer(buffer), uploadHeaps(nullptr), uploadHeapIndex(0)
{
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
