#pragma once

#ifndef _STATICCBUFFER_H_
#define _STATICCBUFFER_H_

#include<Gear/Core/Resource/D3D12Resource/UploadHeap.h>

#include"ImmutableCBuffer.h"

class StaticCBuffer :public ImmutableCBuffer
{
public:

	StaticCBuffer(Buffer* const buffer, const uint32_t size, const bool persistent);

	~StaticCBuffer();

	struct UpdateStruct
	{
		Buffer* const buffer;
		UploadHeap* const uploadHeap;
	}getUpdateStruct(const void* const data, const uint64_t size);

private:

	UploadHeap** uploadHeaps;

};

#endif // !_STATICCBUFFER_H_
