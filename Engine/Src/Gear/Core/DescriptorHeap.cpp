#include<Gear/Core/DescriptorHeap.h>

DescriptorHeap::DescriptorHeap(const UINT numDescriptors, const D3D12_DESCRIPTOR_HEAP_TYPE type, const D3D12_DESCRIPTOR_HEAP_FLAGS flags) :
	numDescriptors(numDescriptors), type(type)
{
	D3D12_DESCRIPTOR_HEAP_DESC desc = {};
	desc.NodeMask = 0;
	desc.NumDescriptors = numDescriptors;
	desc.Type = type;

	GraphicsDevice::get()->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&descriptorHeap));
}

UINT DescriptorHeap::getNumDescriptors()
{
	return numDescriptors;
}

D3D12_DESCRIPTOR_HEAP_TYPE DescriptorHeap::getDescriptorHeapType()
{
	return type;
}

void DescriptorHeap::get()
{
	descriptorHeap.Get();
}
