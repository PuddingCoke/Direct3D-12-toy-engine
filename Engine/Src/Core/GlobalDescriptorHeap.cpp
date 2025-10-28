#include<Gear/Core/GlobalDescriptorHeap.h>

#include<Gear/Core/Internal/GlobalDescriptorHeapInternal.h>

namespace
{
	struct GlobalDescriptorHeapPrivate
	{

		uint32_t resourceIncrementSize = 0;

		uint32_t renderTargetIncrementSize = 0;

		uint32_t depthStencilIncrementSize = 0;

		uint32_t samplerIncrementSize = 0;

		Core::D3D12Core::DescriptorHeap* resourceHeap = nullptr;

		Core::D3D12Core::DescriptorHeap* samplerHeap = nullptr;

		Core::D3D12Core::DescriptorHeap* renderTargetHeap = nullptr;

		Core::D3D12Core::DescriptorHeap* depthStencilHeap = nullptr;

		Core::D3D12Core::DescriptorHeap* nonShaderVisibleResourceHeap = nullptr;

	}pvt;
}

void Core::GlobalDescriptorHeap::Internal::initialize()
{
	pvt.resourceIncrementSize = GraphicsDevice::get()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	pvt.renderTargetIncrementSize = GraphicsDevice::get()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	pvt.depthStencilIncrementSize = GraphicsDevice::get()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);

	pvt.samplerIncrementSize = GraphicsDevice::get()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);

	pvt.resourceHeap = new D3D12Core::DescriptorHeap(1000000, 100000, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE);

	pvt.samplerHeap = new D3D12Core::DescriptorHeap(2048, 0, D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE);

	pvt.renderTargetHeap = new D3D12Core::DescriptorHeap(300000, 30000, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, D3D12_DESCRIPTOR_HEAP_FLAG_NONE);

	pvt.depthStencilHeap = new D3D12Core::DescriptorHeap(300000, 30000, D3D12_DESCRIPTOR_HEAP_TYPE_DSV, D3D12_DESCRIPTOR_HEAP_FLAG_NONE);

	pvt.nonShaderVisibleResourceHeap = new D3D12Core::DescriptorHeap(1000000, 900000, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, D3D12_DESCRIPTOR_HEAP_FLAG_NONE);

	LOGSUCCESS(L"create", LogColor::brightMagenta, L"global descriptor heaps", LogColor::defaultColor, L"succeeded");
}

void Core::GlobalDescriptorHeap::Internal::release()
{
	if (pvt.resourceHeap)
	{
		delete pvt.resourceHeap;
	}

	if (pvt.samplerHeap)
	{
		delete pvt.samplerHeap;
	}

	if (pvt.renderTargetHeap)
	{
		delete pvt.renderTargetHeap;
	}

	if (pvt.depthStencilHeap)
	{
		delete pvt.depthStencilHeap;
	}

	if (pvt.nonShaderVisibleResourceHeap)
	{
		delete pvt.nonShaderVisibleResourceHeap;
	}
}

Core::D3D12Core::DescriptorHeap* Core::GlobalDescriptorHeap::getResourceHeap()
{
	return pvt.resourceHeap;
}

Core::D3D12Core::DescriptorHeap* Core::GlobalDescriptorHeap::getSamplerHeap()
{
	return pvt.samplerHeap;
}

Core::D3D12Core::DescriptorHeap* Core::GlobalDescriptorHeap::getRenderTargetHeap()
{
	return pvt.renderTargetHeap;
}

Core::D3D12Core::DescriptorHeap* Core::GlobalDescriptorHeap::getDepthStencilHeap()
{
	return pvt.depthStencilHeap;
}

Core::D3D12Core::DescriptorHeap* Core::GlobalDescriptorHeap::getNonShaderVisibleResourceHeap()
{
	return pvt.nonShaderVisibleResourceHeap;
}

uint32_t Core::GlobalDescriptorHeap::getResourceIncrementSize()
{
	return pvt.resourceIncrementSize;
}

uint32_t Core::GlobalDescriptorHeap::getRenderTargetIncrementSize()
{
	return pvt.renderTargetIncrementSize;
}

uint32_t Core::GlobalDescriptorHeap::getDepthStencilIncrementSize()
{
	return pvt.depthStencilIncrementSize;
}

uint32_t Core::GlobalDescriptorHeap::getSamplerIncrementSize()
{
	return pvt.samplerIncrementSize;
}
