#pragma once

#ifndef _UPLOADHEAP_H_
#define _UPLOADHEAP_H_

#include<Gear/Core/GraphicsDevice.h>
#include<Gear/Core/GraphicsSettings.h>

#include"Resource.h"

class UploadHeap :public Resource
{
public:

	UploadHeap() = delete;

	void operator=(const UploadHeap&) = delete;

	UploadHeap(const UINT size, const D3D12_HEAP_FLAGS flags = D3D12_HEAP_FLAG_NONE);

	void update(const void* const data, const UINT dataSize);

	void updateGlobalStates() override;

	void resetInternalStates() override;

};

#endif // !_UPLOADHEAP_H_
