#pragma once

#ifndef _DYNAMICCBUFFERREGION_H_
#define _DYNAMICCBUFFERREGION_H_

#include<Gear/Core/Resource/D3D12Resource/UploadHeap.h>

#include<mutex>

class DynamicCBufferRegion
{
public:

	DynamicCBufferRegion(const uint64_t subRegionSize, const uint64_t subRegionNum);

	~DynamicCBufferRegion();

	uint32_t update(const void* const data);

	void reset();

	ID3D12Resource* getResource() const;

	uint64_t getUpdateSize() const;

private:

	const uint64_t subRegionSize;

	UploadHeap** uploadHeap;

	uint8_t** dataPtr;

	std::mutex offsetMutex;

	uint64_t currentOffset;

};

#endif // !_DYNAMICCBUFFERREGION_H_
