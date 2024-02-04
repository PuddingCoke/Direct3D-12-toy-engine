#include<Gear/Core/DescriptorHeap.h>

DescriptorHeap::DescriptorHeap(const UINT numDescriptors, const UINT subRegionSize, const D3D12_DESCRIPTOR_HEAP_TYPE type, const D3D12_DESCRIPTOR_HEAP_FLAGS flags) :
	numDescriptors(numDescriptors), 
	subRegionSize(subRegionSize), 
	type(type), 
	incrementSize(GraphicsDevice::get()->GetDescriptorHandleIncrementSize(type))
{
	D3D12_DESCRIPTOR_HEAP_DESC desc = {};
	desc.NodeMask = 0;
	desc.NumDescriptors = numDescriptors;
	desc.Type = type;
	desc.Flags = flags;

	GraphicsDevice::get()->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&descriptorHeap));

	staticDescriptorPointerStart = descriptorHeap->GetCPUDescriptorHandleForHeapStart();

	dynamicDescriptorPointerStart = descriptorHeap->GetCPUDescriptorHandleForHeapStart();

	dynamicDescriptorPointerStart.Offset(numDescriptors - Graphics::FrameBufferCount * subRegionSize, incrementSize);

	staticDescriptorPointer = staticDescriptorPointerStart;

	dynamicDescriptorPointer = dynamicDescriptorPointerStart;
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
	std::lock_guard<std::mutex> lockGuard(staticDescriptorLock);

	CD3DX12_CPU_DESCRIPTOR_HANDLE retHandle = staticDescriptorPointer;

	staticDescriptorPointer.Offset(num, incrementSize);

	return StaticDescriptorHandle(retHandle, this);
}

DescriptorHeap::DynamicDescriptorHandle DescriptorHeap::allocDynamicDescriptor(UINT num)
{
	std::lock_guard<std::mutex> lockGuard(dynamicDescriptorLock);

	CD3DX12_CPU_DESCRIPTOR_HANDLE retHandle = dynamicDescriptorPointer;

	dynamicDescriptorPointer.Offset(num, incrementSize);

	UINT location = static_cast<UINT>(dynamicDescriptorPointer.ptr - staticDescriptorPointerStart.ptr) / incrementSize;

	if (location >= numDescriptors)
	{
		UINT outBoundNum = location - numDescriptors;

		CD3DX12_CPU_DESCRIPTOR_HANDLE newLocation = dynamicDescriptorPointerStart;

		newLocation.Offset(outBoundNum, incrementSize);

		dynamicDescriptorPointer = newLocation;
	}

	return DynamicDescriptorHandle(retHandle, this);
}

DescriptorHeap::StaticDescriptorHandle::StaticDescriptorHandle(const CD3DX12_CPU_DESCRIPTOR_HANDLE handle, const DescriptorHeap* const descriptorHeap) :
	handle(handle), descriptorHeap(descriptorHeap)
{
}

CD3DX12_CPU_DESCRIPTOR_HANDLE DescriptorHeap::StaticDescriptorHandle::getHandle()
{
	return handle;
}

void DescriptorHeap::StaticDescriptorHandle::move()
{
	handle.Offset(1, descriptorHeap->getIncrementSize());
}

DescriptorHeap::DynamicDescriptorHandle::DynamicDescriptorHandle(const CD3DX12_CPU_DESCRIPTOR_HANDLE handle, const DescriptorHeap* const descriptorHeap) :
	DescriptorHeap::StaticDescriptorHandle(handle, descriptorHeap)
{
}

void DescriptorHeap::DynamicDescriptorHandle::move()
{
	handle.Offset(1, descriptorHeap->getIncrementSize());

	UINT location = static_cast<UINT>(handle.ptr - descriptorHeap->staticDescriptorPointerStart.ptr) / descriptorHeap->incrementSize;

	if (location == descriptorHeap->numDescriptors)
	{
		handle = descriptorHeap->dynamicDescriptorPointerStart;
	}
}
