#pragma once

#ifndef _GLOBALDESCRIPTORHEAP_H_
#define _GLOBALDESCRIPTORHEAP_H_

#include<Gear/Core/DescriptorHeap.h>

using StaticDescriptorHandle = DescriptorHeap::StaticDescriptorHandle;

using DynamicDescriptorHandle = DescriptorHeap::DynamicDescriptorHandle;

class GlobalDescriptorHeap
{
public:

	GlobalDescriptorHeap(const GlobalDescriptorHeap&) = delete;

	void operator=(const GlobalDescriptorHeap&) = delete;

private:

	friend class Gear;

	static GlobalDescriptorHeap* instance;

	GlobalDescriptorHeap();

	~GlobalDescriptorHeap();

	DescriptorHeap* resourceHeap;

	DescriptorHeap* samplerHeap;

	DescriptorHeap* renderTargetHeap;

	DescriptorHeap* depthStencilHeap;

	DescriptorHeap* indexBufferHeap;

};

#endif // !_GLOBALDESCRIPTORHEAP_H_
