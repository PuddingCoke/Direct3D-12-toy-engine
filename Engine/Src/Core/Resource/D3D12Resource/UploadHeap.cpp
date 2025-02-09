#include<Gear/Core/Resource/D3D12Resource/UploadHeap.h>

UploadHeap::UploadHeap(const uint64_t size, const D3D12_HEAP_FLAGS flags) :
	Resource(CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), flags, CD3DX12_RESOURCE_DESC::Buffer(size), false, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr)
{
}

UploadHeap::UploadHeap(UploadHeap* const heap) :
	Resource(heap)
{
}

UploadHeap::~UploadHeap()
{
}

void UploadHeap::update(const void* const data, const uint64_t size) const
{
	const CD3DX12_RANGE readRange(0, 0);
	void* dataPtr = nullptr;
	getResource()->Map(0, &readRange, &dataPtr);
	memcpy(dataPtr, data, size);
	getResource()->Unmap(0, nullptr);
}

void UploadHeap::updateGlobalStates()
{
}

void UploadHeap::resetInternalStates()
{
}

void UploadHeap::resetTransitionStates()
{
}
