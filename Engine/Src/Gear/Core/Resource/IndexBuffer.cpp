#include<Gear/Core/Resource/IndexBuffer.h>

IndexBuffer::IndexBuffer(const DXGI_FORMAT format, const UINT size, const bool stateTracking, const bool cpuWritable, const void* const data, ID3D12GraphicsCommandList6* commandList, std::vector<Resource*>& transientResourcePool)
{
	buffer = new Buffer(size, stateTracking, cpuWritable, data, commandList, transientResourcePool, D3D12_RESOURCE_STATE_INDEX_BUFFER);

	indexBufferView.BufferLocation = buffer->getGPUAddress();
	indexBufferView.Format = format;
	indexBufferView.SizeInBytes = size;
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
}

D3D12_INDEX_BUFFER_VIEW IndexBuffer::getIndexBufferView() const
{
	return indexBufferView;
}

Buffer* IndexBuffer::getBuffer() const
{
	return buffer;
}
