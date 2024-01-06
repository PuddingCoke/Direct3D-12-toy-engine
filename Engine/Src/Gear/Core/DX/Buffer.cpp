#include<Gear/Core/DX/Resource/Buffer.h>

Buffer::Buffer(const UINT size, const BufferUsage usage) :
	Resource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE, &CD3DX12_RESOURCE_DESC::Buffer(size), D3D12_RESOURCE_STATE_COPY_DEST, nullptr),
	uploadHeaps(usage == BufferUsage::STATIC ? nullptr : (new UploadHeap[GraphicsSettings::FrameBufferCount]{ UploadHeap(size),UploadHeap(size), UploadHeap(size) })),
	uploadHeapIndex(0)
{
}

void Buffer::update(const void* const data, const UINT dataSize)
{
	uploadHeaps[uploadHeapIndex].update(data, dataSize);
}

Buffer::~Buffer()
{
	if (uploadHeaps)
	{
		delete[] uploadHeaps;
	}
}
