#pragma once

#ifndef _ENGINERESOURCE_H_
#define _ENGINERESOURCE_H_

#include<Gear/Core/GlobalDescriptorHeap.h>

class EngineResource
{
public:

	EngineResource(const bool persistent);

	virtual ~EngineResource();

	virtual void copyDescriptors();

	bool getPersistent();

protected:

	//copy non shader visible resource heap to shader visible resource heap
	DescriptorHandle getTransientDescriptorHandle() const;

	const bool persistent;

	D3D12_CPU_DESCRIPTOR_HANDLE srvUAVCBVHandleStart;

	uint32_t numSRVUAVCBVDescriptors;

};

#endif // !_ENGINERESOURCE_H_