#include<Gear/Core/DX/Resource/Buffer.h>

Buffer::Buffer(const UINT size, const bool stateTracking, const bool cpuWritable) :
	Resource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE, &CD3DX12_RESOURCE_DESC::Buffer(size), (cpuWritable ? true : false) | stateTracking, D3D12_RESOURCE_STATE_COPY_DEST, nullptr),
	uploadHeaps(cpuWritable ? (new UploadHeap[Graphics::FrameBufferCount]{ UploadHeap(size),UploadHeap(size), UploadHeap(size) }) : nullptr),
	uploadHeapIndex(0),
	globalState(std::make_shared<UINT>(D3D12_RESOURCE_STATE_COPY_DEST)),
	internalState(D3D12_RESOURCE_STATE_COPY_DEST)
{
}

Buffer::Buffer(const Buffer& buff) :
	Resource(buff),
	uploadHeaps(buff.uploadHeaps),
	uploadHeapIndex(buff.uploadHeapIndex),
	globalState(buff.globalState),
	internalState(D3D12_RESOURCE_STATE_UNKNOWN)
{
}

void Buffer::update(const void* const data, const UINT dataSize)
{
	uploadHeaps.get()[uploadHeapIndex].update(data, dataSize);
}

Buffer::~Buffer()
{
}

void Buffer::updateGlobalStates()
{
	if (internalState != D3D12_RESOURCE_STATE_UNKNOWN)
	{
		*globalState = internalState;
	}
}

void Buffer::resetInternalStates()
{
	internalState = D3D12_RESOURCE_STATE_UNKNOWN;
}
