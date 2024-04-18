#pragma once

#ifndef _UPLOADHEAP_H_
#define _UPLOADHEAP_H_

#include<Gear/Core/GraphicsDevice.h>
#include<Gear/Core/Graphics.h>

#include"Resource.h"

class UploadHeap :public Resource
{
public:

	UploadHeap(const UINT64 size, const D3D12_HEAP_FLAGS flags = D3D12_HEAP_FLAG_NONE);

	UploadHeap(UploadHeap&);

	virtual ~UploadHeap();

	void operator=(const UploadHeap&) = delete;

	UploadHeap(const UploadHeap&) = delete;

	void update(const void* const data, const UINT64 size) const;

	void updateGlobalStates() override;

	void resetInternalStates() override;

	void resetTransitionStates() override;

};

#endif // !_UPLOADHEAP_H_
