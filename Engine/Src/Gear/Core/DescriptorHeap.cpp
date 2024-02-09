#include<Gear/Core/DescriptorHeap.h>

DescriptorHeap::DescriptorHeap(const UINT numDescriptors, const UINT subRegionSize, const D3D12_DESCRIPTOR_HEAP_TYPE type, const D3D12_DESCRIPTOR_HEAP_FLAGS flags) :
	numDescriptors(numDescriptors), subRegionSize(subRegionSize), type(type), incrementSize(GraphicsDevice::get()->GetDescriptorHandleIncrementSize(type))
{
	D3D12_DESCRIPTOR_HEAP_DESC desc = {};
	desc.NodeMask = 0;
	desc.NumDescriptors = numDescriptors;
	desc.Type = type;
	desc.Flags = flags;

	GraphicsDevice::get()->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&descriptorHeap));

	staticCPUPointerStart = descriptorHeap->GetCPUDescriptorHandleForHeapStart();

	dynamicCPUPointerStart = descriptorHeap->GetCPUDescriptorHandleForHeapStart();

	dynamicCPUPointerStart.Offset(numDescriptors - Graphics::FrameBufferCount * subRegionSize, incrementSize);

	staticCPUPointer = staticCPUPointerStart;

	dynamicCPUPointer = dynamicCPUPointerStart;

	staticGPUPointerStart = descriptorHeap->GetGPUDescriptorHandleForHeapStart();

	dynamicGPUPointerStart = descriptorHeap->GetGPUDescriptorHandleForHeapStart();

	dynamicGPUPointerStart.Offset(numDescriptors - Graphics::FrameBufferCount * subRegionSize, incrementSize);

	staticGPUPointer = staticGPUPointerStart;

	dynamicGPUPointer = dynamicGPUPointerStart;
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
	descriptorHeap.Get();
}

DescriptorHeap::StaticDescriptorHandle DescriptorHeap::allocStaticDescriptor(UINT num)
{
	std::lock_guard<std::mutex> lockGuard(staticPointerLock);

	CD3DX12_CPU_DESCRIPTOR_HANDLE retCPUHandle = staticCPUPointer;

	CD3DX12_GPU_DESCRIPTOR_HANDLE retGPUHandle = staticGPUPointer;

	staticCPUPointer.Offset(num, incrementSize);

	staticGPUPointer.Offset(num, incrementSize);

	return StaticDescriptorHandle(retCPUHandle, retGPUHandle, this);
}

DescriptorHeap::DynamicDescriptorHandle DescriptorHeap::allocDynamicDescriptor(UINT num)
{
	std::lock_guard<std::mutex> lockGuard(dynamicPointerLock);

	CD3DX12_CPU_DESCRIPTOR_HANDLE retCPUHandle = dynamicCPUPointer;

	CD3DX12_GPU_DESCRIPTOR_HANDLE retGPUHandle = dynamicGPUPointer;

	dynamicCPUPointer.Offset(num, incrementSize);

	dynamicGPUPointer.Offset(num, incrementSize);

	UINT location = static_cast<UINT>(dynamicCPUPointer.ptr - staticCPUPointerStart.ptr) / incrementSize;

	if (location >= numDescriptors)
	{
		UINT outBoundNum = location - numDescriptors;

		CD3DX12_CPU_DESCRIPTOR_HANDLE newCPUPointer = dynamicCPUPointerStart;

		CD3DX12_GPU_DESCRIPTOR_HANDLE newGPUPointer = dynamicGPUPointerStart;

		newCPUPointer.Offset(outBoundNum, incrementSize);

		newGPUPointer.Offset(outBoundNum, incrementSize);

		dynamicCPUPointer = newCPUPointer;

		dynamicGPUPointer = newGPUPointer;
	}

	return DynamicDescriptorHandle(retCPUHandle, retGPUHandle, this);
}

DescriptorHeap::StaticDescriptorHandle::StaticDescriptorHandle(const CD3DX12_CPU_DESCRIPTOR_HANDLE cpuHandle, const CD3DX12_GPU_DESCRIPTOR_HANDLE gpuHandle, const DescriptorHeap* const descriptorHeap) :
	cpuHandle(cpuHandle), gpuHandle(gpuHandle), descriptorHeap(descriptorHeap)
{
}

UINT DescriptorHeap::StaticDescriptorHandle::getCurrentIndex() const
{
	return static_cast<UINT>(cpuHandle.ptr - descriptorHeap->staticCPUPointerStart.ptr) / descriptorHeap->incrementSize;
}

CD3DX12_CPU_DESCRIPTOR_HANDLE DescriptorHeap::StaticDescriptorHandle::getCPUHandle() const
{
	return cpuHandle;
}

CD3DX12_GPU_DESCRIPTOR_HANDLE DescriptorHeap::StaticDescriptorHandle::getGPUHandle() const
{
	return gpuHandle;
}

void DescriptorHeap::StaticDescriptorHandle::move()
{
	cpuHandle.Offset(1, descriptorHeap->incrementSize);

	gpuHandle.Offset(1, descriptorHeap->incrementSize);
}

DescriptorHeap::DynamicDescriptorHandle::DynamicDescriptorHandle(const CD3DX12_CPU_DESCRIPTOR_HANDLE cpuHandle, const CD3DX12_GPU_DESCRIPTOR_HANDLE gpuHandle, const DescriptorHeap* const descriptorHeap) :
	DescriptorHeap::StaticDescriptorHandle(cpuHandle, gpuHandle, descriptorHeap)
{
}

void DescriptorHeap::DynamicDescriptorHandle::move()
{
	cpuHandle.Offset(1, descriptorHeap->incrementSize);
	
	gpuHandle.Offset(1, descriptorHeap->incrementSize);

	UINT location = static_cast<UINT>(cpuHandle.ptr - descriptorHeap->staticCPUPointerStart.ptr) / descriptorHeap->incrementSize;

	if (location == descriptorHeap->numDescriptors)
	{
		cpuHandle = descriptorHeap->dynamicCPUPointerStart;

		gpuHandle = descriptorHeap->dynamicGPUPointerStart;
	}
}
