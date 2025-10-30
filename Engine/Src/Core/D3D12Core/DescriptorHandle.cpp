#include<Gear/Core/D3D12Core/DescriptorHandle.h>

Gear::Core::D3D12Core::DescriptorHandle::DescriptorHandle() :
	cpuHandle{}, gpuHandle{}, currentIndex(), incrementSize()
{
}

Gear::Core::D3D12Core::DescriptorHandle::DescriptorHandle(const CD3DX12_CPU_DESCRIPTOR_HANDLE cpuHandle, const CD3DX12_GPU_DESCRIPTOR_HANDLE gpuHandle,
	const uint32_t currentIndex, const uint32_t incrementSize) :
	cpuHandle(cpuHandle), gpuHandle(gpuHandle), currentIndex(currentIndex), incrementSize(incrementSize)
{
}

CD3DX12_CPU_DESCRIPTOR_HANDLE Gear::Core::D3D12Core::DescriptorHandle::getCPUHandle() const
{
	return cpuHandle;
}

CD3DX12_GPU_DESCRIPTOR_HANDLE Gear::Core::D3D12Core::DescriptorHandle::getGPUHandle() const
{
	return gpuHandle;
}

uint32_t Gear::Core::D3D12Core::DescriptorHandle::getCurrentIndex() const
{
	return currentIndex;
}

void Gear::Core::D3D12Core::DescriptorHandle::move(const uint32_t num)
{
	cpuHandle.Offset(num, incrementSize);

	gpuHandle.Offset(num, incrementSize);

	currentIndex += num;
}