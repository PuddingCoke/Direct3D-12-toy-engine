#pragma once

#ifndef _GLOBALDESCRIPTORHEAP_H_
#define _GLOBALDESCRIPTORHEAP_H_

#include<Gear/Core/DescriptorHeap.h>

class GlobalDescriptorHeap :public DescriptorHeap
{
public:

	GlobalDescriptorHeap(const GlobalDescriptorHeap&) = delete;

	void operator=(const GlobalDescriptorHeap&) = delete;

private:

	GlobalDescriptorHeap();

	static GlobalDescriptorHeap* instance;

	static constexpr UINT DescriptorHeapSize = 1000000;

	static constexpr UINT PerSubRegionSize = 1000;

};

#endif // !_GLOBALDESCRIPTORHEAP_H_
