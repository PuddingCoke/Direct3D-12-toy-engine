﻿#pragma once

#ifndef _READBACKHEAP_H_
#define _READBACKHEAP_H_

#include"Resource.h"

class ReadbackHeap :public Resource
{
public:

	ReadbackHeap() = delete;

	ReadbackHeap(const ReadbackHeap&) = delete;

	void operator=(const ReadbackHeap&) = delete;

	ReadbackHeap(const uint64_t size, const D3D12_HEAP_FLAGS flags = D3D12_HEAP_FLAG_NONE);

	ReadbackHeap(ReadbackHeap* const);

	virtual ~ReadbackHeap();

	void* map(const D3D12_RANGE readRange) const;

	void unmap() const;

	void updateGlobalStates() override;

	void resetInternalStates() override;

protected:

	void resetTransitionStates() override;

};

#endif // !_READBACKHEAP_H_