#pragma once

#ifndef _GEAR_CORE_D3D12CORE_DESCRIPTORHANDLE_H_
#define _GEAR_CORE_D3D12CORE_DESCRIPTORHANDLE_H_

#include<D3D12Headers/d3dx12.h>

namespace Gear
{
	namespace Core
	{
		namespace D3D12Core
		{
			class DescriptorHandle
			{
			public:

				DescriptorHandle();

				DescriptorHandle(const CD3DX12_CPU_DESCRIPTOR_HANDLE cpuHandle, const CD3DX12_GPU_DESCRIPTOR_HANDLE gpuHandle,
					const uint32_t currentIndex, const uint32_t incrementSize);

				CD3DX12_CPU_DESCRIPTOR_HANDLE getCPUHandle() const;

				CD3DX12_GPU_DESCRIPTOR_HANDLE getGPUHandle() const;

				uint32_t getCurrentIndex() const;

				void move(const uint32_t num = 1u);

			protected:

				CD3DX12_CPU_DESCRIPTOR_HANDLE cpuHandle;

				CD3DX12_GPU_DESCRIPTOR_HANDLE gpuHandle;

				uint32_t currentIndex;

				uint32_t incrementSize;

			};
		}
	}
}

#endif // !_GEAR_CORE_D3D12CORE_DESCRIPTORHANDLE_H_
