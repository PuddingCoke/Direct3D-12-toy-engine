#include<Gear/Core/Resource/IndexBuffer.h>

IndexBuffer::IndexBuffer(Buffer* const buffer, const DXGI_FORMAT format, const UINT size, const bool cpuWritable) :
	buffer(buffer), uploadHeaps(nullptr), uploadHeapIndex(0)
{
	indexBufferView.BufferLocation = buffer->getGPUAddress();
	indexBufferView.Format = format;
	indexBufferView.SizeInBytes = size;

	if (cpuWritable)
	{
		uploadHeaps = new UploadHeap * [Graphics::FrameBufferCount];

		for (UINT i = 0; i < Graphics::FrameBufferCount; i++)
		{
			uploadHeaps[i] = new UploadHeap(size);
		}
	}
}

IndexBuffer::IndexBuffer(const IndexBuffer& ib) :
	indexBufferView(ib.indexBufferView),
	uploadHeaps(ib.uploadHeaps),
	uploadHeapIndex(ib.uploadHeapIndex),
	buffer(new Buffer(*(ib.buffer)))
{
}

IndexBuffer::~IndexBuffer()
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

IndexBufferDesc IndexBuffer::getIndexBuffer() const
{
	IndexBufferDesc desc = {};
	desc.buffer = buffer;
	desc.ibv = indexBufferView;

	return desc;
}

Buffer* IndexBuffer::getBuffer() const
{
	return buffer;
}
