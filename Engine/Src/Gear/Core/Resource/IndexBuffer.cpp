#include<Gear/Core/Resource/IndexBuffer.h>

IndexBuffer::IndexBuffer(const DXGI_FORMAT format, const UINT size, const bool cpuWritable, const void* const data, ID3D12GraphicsCommandList6* commandList, std::vector<Resource*>* transientResourcePool) :
	uploadHeaps(nullptr), uploadHeapIndex(0)
{
	buffer = new Buffer(size, cpuWritable, data, commandList, transientResourcePool, D3D12_RESOURCE_STATE_INDEX_BUFFER);

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

D3D12_INDEX_BUFFER_VIEW IndexBuffer::getIndexBufferView() const
{
	return indexBufferView;
}

Buffer* IndexBuffer::getBuffer() const
{
	return buffer;
}
