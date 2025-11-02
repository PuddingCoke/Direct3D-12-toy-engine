#include<Gear/Core/GlobalDescriptorHeap.h>

#include<Gear/Core/Internal/GlobalDescriptorHeapInternal.h>

namespace
{
	struct GlobalDescriptorHeapPrivate
	{

		Gear::Core::D3D12Core::DescriptorHeap* resourceHeap = nullptr;

		Gear::Core::D3D12Core::DescriptorHeap* stagingResourceHeap = nullptr;

		Gear::Core::D3D12Core::DescriptorHeap* samplerHeap = nullptr;

		Gear::Core::D3D12Core::DescriptorHeap* renderTargetHeap = nullptr;

		Gear::Core::D3D12Core::DescriptorHeap* depthStencilHeap = nullptr;

	}pvt;
}

void Gear::Core::GlobalDescriptorHeap::Internal::initialize()
{
	pvt.resourceHeap = new D3D12Core::DescriptorHeap(1000000, 100000, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE);

	pvt.stagingResourceHeap = new D3D12Core::DescriptorHeap(1000000, 900000, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, D3D12_DESCRIPTOR_HEAP_FLAG_NONE);

	pvt.samplerHeap = new D3D12Core::DescriptorHeap(2048, 0, D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE);

	pvt.renderTargetHeap = new D3D12Core::DescriptorHeap(300000, 30000, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, D3D12_DESCRIPTOR_HEAP_FLAG_NONE);

	pvt.depthStencilHeap = new D3D12Core::DescriptorHeap(300000, 30000, D3D12_DESCRIPTOR_HEAP_TYPE_DSV, D3D12_DESCRIPTOR_HEAP_FLAG_NONE);

	LOGSUCCESS(L"create", LogColor::brightMagenta, L"global descriptor heaps", LogColor::defaultColor, L"succeeded");
}

void Gear::Core::GlobalDescriptorHeap::Internal::release()
{
	if (pvt.resourceHeap)
	{
		delete pvt.resourceHeap;
	}

	if (pvt.stagingResourceHeap)
	{
		delete pvt.stagingResourceHeap;
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
}

Gear::Core::D3D12Core::DescriptorHeap* Gear::Core::GlobalDescriptorHeap::getResourceHeap()
{
	return pvt.resourceHeap;
}

Gear::Core::D3D12Core::DescriptorHeap* Gear::Core::GlobalDescriptorHeap::getStagingResourceHeap()
{
	return pvt.stagingResourceHeap;
}

Gear::Core::D3D12Core::DescriptorHeap* Gear::Core::GlobalDescriptorHeap::getSamplerHeap()
{
	return pvt.samplerHeap;
}

Gear::Core::D3D12Core::DescriptorHeap* Gear::Core::GlobalDescriptorHeap::getRenderTargetHeap()
{
	return pvt.renderTargetHeap;
}

Gear::Core::D3D12Core::DescriptorHeap* Gear::Core::GlobalDescriptorHeap::getDepthStencilHeap()
{
	return pvt.depthStencilHeap;
}

