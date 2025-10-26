#pragma once

#ifndef _CORE_GLOBALDESCRIPTORHEAP_H_
#define _CORE_GLOBALDESCRIPTORHEAP_H_

#include<Gear/Core/DescriptorHeap.h>

namespace Core
{
	namespace GlobalDescriptorHeap
	{
		DescriptorHeap* getResourceHeap();

		DescriptorHeap* getSamplerHeap();

		DescriptorHeap* getRenderTargetHeap();

		DescriptorHeap* getDepthStencilHeap();

		DescriptorHeap* getNonShaderVisibleResourceHeap();

		uint32_t getResourceIncrementSize();

		uint32_t getRenderTargetIncrementSize();

		uint32_t getDepthStencilIncrementSize();

		uint32_t getSamplerIncrementSize();
	}
}

#endif // !_CORE_GLOBALDESCRIPTORHEAP_H_