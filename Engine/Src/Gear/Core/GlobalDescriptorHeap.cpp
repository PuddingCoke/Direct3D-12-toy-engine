#include<Gear/Core/GlobalDescriptorHeap.h>

GlobalDescriptorHeap* GlobalDescriptorHeap::instance = nullptr;

GlobalDescriptorHeap::GlobalDescriptorHeap()
{
	resourceHeap = new DescriptorHeap(1000000, 1000, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE);

	samplerHeap = new DescriptorHeap(512, 0, D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE);

	renderTargetHeap = new DescriptorHeap(100000, 100, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, D3D12_DESCRIPTOR_HEAP_FLAG_NONE);

	depthStencilHeap = new DescriptorHeap(100000, 100, D3D12_DESCRIPTOR_HEAP_TYPE_DSV, D3D12_DESCRIPTOR_HEAP_FLAG_NONE);

	indexBufferHeap = new DescriptorHeap(300000, 300, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, D3D12_DESCRIPTOR_HEAP_FLAG_NONE);
}

GlobalDescriptorHeap::~GlobalDescriptorHeap()
{
	if (resourceHeap)
	{
		delete resourceHeap;
	}

	if (samplerHeap)
	{
		delete samplerHeap;
	}

	if (renderTargetHeap)
	{
		delete renderTargetHeap;
	}

	if (depthStencilHeap)
	{
		delete depthStencilHeap;
	}

	if (indexBufferHeap)
	{
		delete indexBufferHeap;
	}
}
