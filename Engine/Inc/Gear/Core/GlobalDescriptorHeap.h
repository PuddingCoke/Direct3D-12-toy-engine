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

	static uint32_t getResourceIncrementSize();

	static uint32_t getRenderTargetIncrementSize();

	static uint32_t getDepthStencilIncrementSize();

	static uint32_t getSamplerIncrementSize();

private:

	friend class RenderEngine;

	static GlobalDescriptorHeap* instance;

	static uint32_t resourceIncrementSize;

	static uint32_t renderTargetIncrementSize;

	static uint32_t depthStencilIncrementSize;

	static uint32_t samplerIncrementSize;

	GlobalDescriptorHeap();

	~GlobalDescriptorHeap();

	DescriptorHeap* resourceHeap;

	DescriptorHeap* samplerHeap;

	DescriptorHeap* renderTargetHeap;

	DescriptorHeap* depthStencilHeap;

	DescriptorHeap* nonShaderVisibleResourceHeap;

};

#endif // !_GLOBALDESCRIPTORHEAP_H_