#include<Gear/Core/GlobalDescriptorHeap.h>

GlobalDescriptorHeap* GlobalDescriptorHeap::instance = nullptr;

GlobalDescriptorHeap::GlobalDescriptorHeap()
{
	resourceHeap = new DescriptorHeap(1000000, 100000, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE);

	samplerHeap = new DescriptorHeap(2048, 0, D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE);

	renderTargetHeap = new DescriptorHeap(300000, 30000, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, D3D12_DESCRIPTOR_HEAP_FLAG_NONE);

	depthStencilHeap = new DescriptorHeap(300000, 30000, D3D12_DESCRIPTOR_HEAP_TYPE_DSV, D3D12_DESCRIPTOR_HEAP_FLAG_NONE);

	nonShaderVisibleResourceHeap = new DescriptorHeap(1000000, 900000, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, D3D12_DESCRIPTOR_HEAP_FLAG_NONE);

	std::cout << "[class GlobalDescriptorHeap] create global descriptor heaps succeeded\n";
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

	if (nonShaderVisibleResourceHeap)
	{
		delete nonShaderVisibleResourceHeap;
	}
}

DescriptorHeap* GlobalDescriptorHeap::getResourceHeap()
{
	return instance->resourceHeap;
}

DescriptorHeap* GlobalDescriptorHeap::getSamplerHeap()
{
	return instance->samplerHeap;
}

DescriptorHeap* GlobalDescriptorHeap::getRenderTargetHeap()
{
	return instance->renderTargetHeap;
}

DescriptorHeap* GlobalDescriptorHeap::getDepthStencilHeap()
{
	return instance->depthStencilHeap;
}

DescriptorHeap* GlobalDescriptorHeap::getNonShaderVisibleResourceHeap()
{
	return instance->nonShaderVisibleResourceHeap;
}
