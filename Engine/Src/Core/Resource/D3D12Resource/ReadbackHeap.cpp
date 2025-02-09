﻿#include<Gear/Core/Resource/D3D12Resource/ReadbackHeap.h>

ReadbackHeap::ReadbackHeap(const uint64_t size, const D3D12_HEAP_FLAGS flags) :
	Resource(CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_READBACK), flags, CD3DX12_RESOURCE_DESC::Buffer(size), false, D3D12_RESOURCE_STATE_COPY_DEST, nullptr)
{
}

ReadbackHeap::ReadbackHeap(ReadbackHeap* const heap) :
	Resource(heap)
{
}

void* ReadbackHeap::map(const D3D12_RANGE readRange) const
{
	void* dataPtr = nullptr;

	getResource()->Map(0, &readRange, &dataPtr);

	return dataPtr;
}

void ReadbackHeap::unmap() const
{
	getResource()->Unmap(0, nullptr);
}

void ReadbackHeap::updateGlobalStates()
{
}

void ReadbackHeap::resetInternalStates()
{
}

void ReadbackHeap::resetTransitionStates()
{
}

ReadbackHeap::~ReadbackHeap()
{
}
