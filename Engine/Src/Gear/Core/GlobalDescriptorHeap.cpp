#include<Gear/Core/GlobalDescriptorHeap.h>

GlobalDescriptorHeap* GlobalDescriptorHeap::instance = nullptr;

uint32_t GlobalDescriptorHeap::resourceIncrementSize = 0;

uint32_t GlobalDescriptorHeap::renderTargetIncrementSize = 0;

uint32_t GlobalDescriptorHeap::depthStencilIncrementSize = 0;

uint32_t GlobalDescriptorHeap::samplerIncrementSize = 0;

GlobalDescriptorHeap::GlobalDescriptorHeap()
{
	resourceIncrementSize = GraphicsDevice::get()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	renderTargetIncrementSize = GraphicsDevice::get()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	depthStencilIncrementSize = GraphicsDevice::get()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);

	samplerIncrementSize = GraphicsDevice::get()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);

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

uint32_t GlobalDescriptorHeap::getResourceIncrementSize()
{
	return resourceIncrementSize;
}

uint32_t GlobalDescriptorHeap::getRenderTargetIncrementSize()
{
	return renderTargetIncrementSize;
}

uint32_t GlobalDescriptorHeap::getDepthStencilIncrementSize()
{
	return depthStencilIncrementSize;
}

uint32_t GlobalDescriptorHeap::getSamplerIncrementSize()
{
	return samplerIncrementSize;
}
