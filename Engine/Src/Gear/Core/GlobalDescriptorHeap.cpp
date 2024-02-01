#include<Gear/Core/GlobalDescriptorHeap.h>

GlobalDescriptorHeap* GlobalDescriptorHeap::instance = nullptr;

GlobalDescriptorHeap::GlobalDescriptorHeap() :
	DescriptorHeap(DescriptorHeapSize, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)
{
}
