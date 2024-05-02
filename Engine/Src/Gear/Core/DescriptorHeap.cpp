#include<Gear/Core/DescriptorHeap.h>

DescriptorHeap::DescriptorHeap(const UINT numDescriptors, const UINT subRegionSize, const D3D12_DESCRIPTOR_HEAP_TYPE type, const D3D12_DESCRIPTOR_HEAP_FLAGS flags) :
	numDescriptors(numDescriptors), subRegionSize(subRegionSize), type(type),
	incrementSize(GraphicsDevice::get()->GetDescriptorHandleIncrementSize(type))
{
	D3D12_DESCRIPTOR_HEAP_DESC desc = {};
	desc.NodeMask = 0;
	desc.NumDescriptors = numDescriptors;
	desc.Type = type;
	desc.Flags = flags;

	GraphicsDevice::get()->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&descriptorHeap));

	staticCPUPointerStart = descriptorHeap->GetCPUDescriptorHandleForHeapStart();

	dynamicCPUPointerStart = descriptorHeap->GetCPUDescriptorHandleForHeapStart();

	dynamicCPUPointerStart.Offset(numDescriptors - subRegionSize, incrementSize);

	staticCPUPointer = staticCPUPointerStart;

	dynamicCPUPointer = dynamicCPUPointerStart;

	if (flags == D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE)
	{
		staticGPUPointerStart = descriptorHeap->GetGPUDescriptorHandleForHeapStart();

		dynamicGPUPointerStart = descriptorHeap->GetGPUDescriptorHandleForHeapStart();

		dynamicGPUPointerStart.Offset(numDescriptors - subRegionSize, incrementSize);

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

UINT DescriptorHeap::getNumDescriptors() const
{
	return numDescriptors;
}

UINT DescriptorHeap::getSubRegionSize() const
{
	return subRegionSize;
}

D3D12_DESCRIPTOR_HEAP_TYPE DescriptorHeap::getDescriptorHeapType() const
{
	return type;
}

UINT DescriptorHeap::getIncrementSize() const
{
	return incrementSize;
}

ID3D12DescriptorHeap* DescriptorHeap::get() const
{
	return descriptorHeap.Get();
}

DescriptorHeap::DescriptorHandle DescriptorHeap::allocStaticDescriptor(const UINT num)
{
	std::lock_guard<std::mutex> lockGuard(staticPointerLock);

	const CD3DX12_CPU_DESCRIPTOR_HANDLE retCPUHandle = staticCPUPointer;

	const CD3DX12_GPU_DESCRIPTOR_HANDLE retGPUHandle = staticGPUPointer;

	staticCPUPointer.Offset(num, incrementSize);

	staticGPUPointer.Offset(num, incrementSize);

	return DescriptorHandle(retCPUHandle, retGPUHandle, this);
}

DescriptorHeap::DescriptorHandle DescriptorHeap::allocDynamicDescriptor(const UINT num)
{
	std::lock_guard<std::mutex> lockGuard(dynamicPointerLock);

	//if there is no enough room for target descriptor num just reset cpuPointer and gpuPointer
	
	const UINT movedLocation = static_cast<UINT>(dynamicCPUPointer.ptr - staticCPUPointerStart.ptr) / incrementSize + num;

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

DescriptorHeap::DescriptorHandle::DescriptorHandle() :
	cpuHandle{}, gpuHandle{}, descriptorHeap(nullptr)
{
}

DescriptorHeap::DescriptorHandle::DescriptorHandle(const CD3DX12_CPU_DESCRIPTOR_HANDLE cpuHandle, const CD3DX12_GPU_DESCRIPTOR_HANDLE gpuHandle, const DescriptorHeap* const descriptorHeap) :
	cpuHandle(cpuHandle), gpuHandle(gpuHandle), descriptorHeap(descriptorHeap)
{
}

UINT DescriptorHeap::DescriptorHandle::getCurrentIndex() const
{
	return static_cast<UINT>(cpuHandle.ptr - descriptorHeap->staticCPUPointerStart.ptr) / descriptorHeap->incrementSize;
}

CD3DX12_CPU_DESCRIPTOR_HANDLE DescriptorHeap::DescriptorHandle::getCPUHandle() const
{
	return cpuHandle;
}

CD3DX12_GPU_DESCRIPTOR_HANDLE DescriptorHeap::DescriptorHandle::getGPUHandle() const
{
	return gpuHandle;
}

void DescriptorHeap::DescriptorHandle::move()
{
	cpuHandle.Offset(1, descriptorHeap->incrementSize);

	gpuHandle.Offset(1, descriptorHeap->incrementSize);
}

void DescriptorHeap::DescriptorHandle::offset(const UINT num)
{
	cpuHandle.Offset(num, descriptorHeap->incrementSize);

	gpuHandle.Offset(num, descriptorHeap->incrementSize);
}
