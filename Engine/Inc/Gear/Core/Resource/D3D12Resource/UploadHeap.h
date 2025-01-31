#pragma once

#ifndef _UPLOADHEAP_H_
#define _UPLOADHEAP_H_

#include"Resource.h"

class UploadHeap :public Resource
{
public:

	UploadHeap() = delete;

	UploadHeap(const UploadHeap&) = delete;

	void operator=(const UploadHeap&) = delete;

	UploadHeap(const uint64_t size, const D3D12_HEAP_FLAGS flags = D3D12_HEAP_FLAG_NONE);

	UploadHeap(UploadHeap&);

	virtual ~UploadHeap();

	void update(const void* const data, const uint64_t size) const;

	void updateGlobalStates() override;

	void resetInternalStates() override;

protected:

	void resetTransitionStates() override;

};

#endif // !_UPLOADHEAP_H_