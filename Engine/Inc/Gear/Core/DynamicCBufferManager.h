#pragma once

#ifndef _CORE_DYNAMICCBUFFERMANAGER_H_
#define _CORE_DYNAMICCBUFFERMANAGER_H_

#include<D3D12Headers/d3dx12.h>

namespace Core
{
	namespace DynamicCBufferManager
	{
		struct AvailableDescriptor
		{
			//provide gpu address to directly bind constant buffer
			const D3D12_GPU_VIRTUAL_ADDRESS gpuAddress;
			//provide index to directly access resource in resource descriptor heap
			const uint32_t descriptorIndex;
		} requestDescriptor(const uint32_t regionIndex, const void* const data);
	}
}

#endif // !_CORE_DYNAMICCBUFFERMANAGER_H_