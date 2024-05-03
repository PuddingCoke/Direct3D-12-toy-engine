#pragma once

#ifndef _GLOBALDESCRIPTORHEAP_H_
#define _GLOBALDESCRIPTORHEAP_H_

#include<Gear/Core/DescriptorHeap.h>

class GlobalDescriptorHeap
{
public:

	GlobalDescriptorHeap(const GlobalDescriptorHeap&) = delete;

	void operator=(const GlobalDescriptorHeap&) = delete;

	static DescriptorHeap* getResourceHeap();

	static DescriptorHeap* getSamplerHeap();

	static DescriptorHeap* getRenderTargetHeap();

	static DescriptorHeap* getDepthStencilHeap();

	static DescriptorHeap* getNonShaderVisibleResourceHeap();

	static UINT getResourceIncrementSize();

	static UINT getRenderTargetIncrementSize();

	static UINT getDepthStencilIncrementSize();

	static UINT getSamplerIncrementSize();

private:

	friend class RenderEngine;

	static GlobalDescriptorHeap* instance;

	static UINT resourceIncrementSize;

	static UINT renderTargetIncrementSize;

	static UINT depthStencilIncrementSize;

	static UINT samplerIncrementSize;

	GlobalDescriptorHeap();

	~GlobalDescriptorHeap();

	DescriptorHeap* resourceHeap;

	DescriptorHeap* samplerHeap;

	DescriptorHeap* renderTargetHeap;

	DescriptorHeap* depthStencilHeap;

	DescriptorHeap* nonShaderVisibleResourceHeap;

};

#endif // !_GLOBALDESCRIPTORHEAP_H_
