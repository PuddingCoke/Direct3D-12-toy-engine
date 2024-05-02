#pragma once

#ifndef _ENGINERESOURCE_H_
#define _ENGINERESOURCE_H_

#include<Gear/Core/GlobalDescriptorHeap.h>

class EngineResource
{
public:

	EngineResource();

	virtual ~EngineResource();

	virtual void copyDescriptors();

protected:

	//use by non persistent resource only

	//descriptor handle in non shader visible resource heap
	D3D12_CPU_DESCRIPTOR_HANDLE srvUAVCBVHandleStart;

	//number of descriptors allocated from non shader visible resource heap
	UINT numSRVUAVCBVDescriptors;

};

#endif // !_ENGINERESOURCE_H_
