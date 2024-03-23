#pragma once

#ifndef _READBACKHEAP_H_
#define _READBACKHEAP_H_

#include"Resource.h"

class ReadbackHeap :public Resource
{
public:

	ReadbackHeap(const UINT size, const D3D12_HEAP_FLAGS flags = D3D12_HEAP_FLAG_NONE);

	ReadbackHeap(ReadbackHeap&);

	virtual ~ReadbackHeap();

	void operator=(const ReadbackHeap&) = delete;

	ReadbackHeap(const ReadbackHeap&) = delete;

	void* map(const D3D12_RANGE readRange);

	void unmap();

	void updateGlobalStates() override;

	void resetInternalStates() override;

	void resetTransitionStates() override;

};

#endif // !_READBACKHEAP_H_
