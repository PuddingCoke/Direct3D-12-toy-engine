﻿#pragma once

#ifndef _GEAR_CORE_DYNAMICCBUFFERMANAGER_H_
#define _GEAR_CORE_DYNAMICCBUFFERMANAGER_H_

#include<D3D12Headers/d3dx12.h>

namespace Gear
{
	namespace Core
	{
		namespace DynamicCBufferManager
		{
			struct AvailableLocation
			{
				//提供数据指针用于更新内容
				void* const dataPtr;
				//提供GPU地址来直接绑定常量缓冲
				const D3D12_GPU_VIRTUAL_ADDRESS gpuAddress;
				//提供资源视图的位置来通过ResourceDescriptorHeap访问常量缓冲
				const uint32_t descriptorIndex;
			} requestLocation(const uint32_t regionIndex);
		}
	}
}

#endif // !_GEAR_CORE_DYNAMICCBUFFERMANAGER_H_