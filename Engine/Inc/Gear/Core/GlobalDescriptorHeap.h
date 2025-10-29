#pragma once

#ifndef _GEAR_CORE_GLOBALDESCRIPTORHEAP_H_
#define _GEAR_CORE_GLOBALDESCRIPTORHEAP_H_

#include<Gear/Core/D3D12Core/DescriptorHeap.h>

namespace Gear
{
	namespace Core
	{
		namespace GlobalDescriptorHeap
		{
			D3D12Core::DescriptorHeap* getResourceHeap();

			D3D12Core::DescriptorHeap* getSamplerHeap();

			D3D12Core::DescriptorHeap* getRenderTargetHeap();

			D3D12Core::DescriptorHeap* getDepthStencilHeap();

			D3D12Core::DescriptorHeap* getNonShaderVisibleResourceHeap();

			uint32_t getResourceIncrementSize();

			uint32_t getRenderTargetIncrementSize();

			uint32_t getDepthStencilIncrementSize();

			uint32_t getSamplerIncrementSize();
		}
	}
}

#endif // !_GEAR_CORE_GLOBALDESCRIPTORHEAP_H_