#include<Gear/Core/D3D12Core/DescriptorHeap.h>

Gear::Core::D3D12Core::DescriptorHeap::DescriptorHeap(const uint32_t numDescriptors, const uint32_t subRegionSize, const D3D12_DESCRIPTOR_HEAP_TYPE type, const D3D12_DESCRIPTOR_HEAP_FLAGS flags) :
	numDescriptors(numDescriptors), subRegionSize(subRegionSize), type(type),
	incrementSize(Gear::Core::GraphicsDevice::get()->GetDescriptorHandleIncrementSize(type))
{
	D3D12_DESCRIPTOR_HEAP_DESC desc = {};
	desc.NodeMask = 0;
	desc.NumDescriptors = numDescriptors;
	desc.Type = type;
	desc.Flags = flags;

	Gear::Core::GraphicsDevice::get()->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&descriptorHeap));

	staticCPUPointerStart = descriptorHeap->GetCPUDescriptorHandleForHeapStart();

	dynamicCPUPointerStart = CD3DX12_CPU_DESCRIPTOR_HANDLE(descriptorHeap->GetCPUDescriptorHandleForHeapStart(), numDescriptors - subRegionSize, incrementSize);

	staticCPUPointer = staticCPUPointerStart;

	dynamicCPUPointer = dynamicCPUPointerStart;

	if (flags == D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE)
	{
		staticGPUPointerStart = descriptorHeap->GetGPUDescriptorHandleForHeapStart();

		dynamicGPUPointerStart = CD3DX12_GPU_DESCRIPTOR_HANDLE(descriptorHeap->GetGPUDescriptorHandleForHeapStart(), numDescriptors - subRegionSize, incrementSize);

		staticGPUPointer = staticGPUPointerStart;

		dynamicGPUPointer = dynamicGPUPointerStart;
	}
	else
	{
		staticGPUPointerStart = CD3DX12_GPU_DESCRIPTOR_HANDLE();

		dynamicGPUPointerStart = CD3DX12_GPU_DESCRIPTOR_HANDLE();

		staticGPUPointer = CD3DX12_GPU_DESCRIPTOR_HANDLE();

		dynamicGPUPointer = CD3DX12_GPU_DESCRIPTOR_HANDLE();
	}
}

uint32_t Gear::Core::D3D12Core::DescriptorHeap::getNumDescriptors() const
{
	return numDescriptors;
}

uint32_t Gear::Core::D3D12Core::DescriptorHeap::getSubRegionSize() const
{
	return subRegionSize;
}

D3D12_DESCRIPTOR_HEAP_TYPE Gear::Core::D3D12Core::DescriptorHeap::getDescriptorHeapType() const
{
	return type;
}

uint32_t Gear::Core::D3D12Core::DescriptorHeap::getIncrementSize() const
{
	return incrementSize;
}

ID3D12DescriptorHeap* Gear::Core::D3D12Core::DescriptorHeap::get() const
{
	return descriptorHeap.Get();
}

Gear::Core::D3D12Core::DescriptorHandle Gear::Core::D3D12Core::DescriptorHeap::allocStaticDescriptor(const uint32_t num)
{
	std::lock_guard<std::mutex> lockGuard(staticPointerLock);

	const CD3DX12_CPU_DESCRIPTOR_HANDLE retCPUHandle = staticCPUPointer;

	const CD3DX12_GPU_DESCRIPTOR_HANDLE retGPUHandle = staticGPUPointer;

	staticCPUPointer.Offset(num, incrementSize);

	staticGPUPointer.Offset(num, incrementSize);

	return DescriptorHandle(retCPUHandle, retGPUHandle, this);
}

Gear::Core::D3D12Core::DescriptorHandle Gear::Core::D3D12Core::DescriptorHeap::allocDynamicDescriptor(const uint32_t num)
{
	std::lock_guard<std::mutex> lockGuard(dynamicPointerLock);

	//if there is no enough room for target descriptor num just reset cpuPointer and gpuPointer
	
	const uint32_t movedLocation = static_cast<uint32_t>(dynamicCPUPointer.ptr - staticCPUPointerStart.ptr) / incrementSize + num;

	if (movedLocation > numDescriptors)
	{
		dynamicCPUPointer = dynamicCPUPointerStart;

		dynamicGPUPointer = dynamicGPUPointerStart;
	}

	const CD3DX12_CPU_DESCRIPTOR_HANDLE retCPUHandle = dynamicCPUPointer;

	const CD3DX12_GPU_DESCRIPTOR_HANDLE retGPUHandle = dynamicGPUPointer;

	dynamicCPUPointer.Offset(num, incrementSize);

	dynamicGPUPointer.Offset(num, incrementSize);

	return DescriptorHandle(retCPUHandle, retGPUHandle, this);
}

Gear::Core::D3D12Core::DescriptorHandle::DescriptorHandle() :
	cpuHandle{}, gpuHandle{}, descriptorHeap(nullptr)
{
}

Gear::Core::D3D12Core::DescriptorHandle::DescriptorHandle(const CD3DX12_CPU_DESCRIPTOR_HANDLE cpuHandle, const CD3DX12_GPU_DESCRIPTOR_HANDLE gpuHandle, const DescriptorHeap* const descriptorHeap) :
	cpuHandle(cpuHandle), gpuHandle(gpuHandle), descriptorHeap(descriptorHeap)
{
}

uint32_t Gear::Core::D3D12Core::DescriptorHandle::getCurrentIndex() const
{
	return static_cast<uint32_t>(cpuHandle.ptr - descriptorHeap->staticCPUPointerStart.ptr) / descriptorHeap->incrementSize;
}

CD3DX12_CPU_DESCRIPTOR_HANDLE Gear::Core::D3D12Core::DescriptorHandle::getCPUHandle() const
{
	return cpuHandle;
}

CD3DX12_GPU_DESCRIPTOR_HANDLE Gear::Core::D3D12Core::DescriptorHandle::getGPUHandle() const
{
	return gpuHandle;
}

void Gear::Core::D3D12Core::DescriptorHandle::move()
{
	cpuHandle.Offset(1, descriptorHeap->incrementSize);

	gpuHandle.Offset(1, descriptorHeap->incrementSize);
}

void Gear::Core::D3D12Core::DescriptorHandle::offset(const uint32_t num)
{
	cpuHandle.Offset(num, descriptorHeap->incrementSize);

	gpuHandle.Offset(num, descriptorHeap->incrementSize);
}
