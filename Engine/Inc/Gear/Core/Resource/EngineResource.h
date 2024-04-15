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

	CD3DX12_CPU_DESCRIPTOR_HANDLE srvUAVCBVHandleStart;

	UINT numSRVUAVCBVDescriptors;

};

#endif // !_ENGINERESOURCE_H_
