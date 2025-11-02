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
	{
		const uint32_t numStaticSRVDescriptors = 100000u;

		{
			const uint32_t numResourceHeapDescriptors = 1000000u;

			const uint32_t numStaticSRVUAVDescriptors = numStaticSRVDescriptors * 2u;

			pvt.resourceHeap = new D3D12Core::DescriptorHeap(numResourceHeapDescriptors, numResourceHeapDescriptors - numStaticSRVUAVDescriptors, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE);
		}

		{
			const uint32_t numStagingResourceDescriptors = 2000000u;

			//由于持久性资源基本上不会在StagingResourceHeap的静态区域创建描述符
			//因此StagingResourceHeap的动态区域可以给得非常大
			pvt.stagingResourceHeap = new D3D12Core::DescriptorHeap(numStagingResourceDescriptors + numStaticSRVDescriptors, numStagingResourceDescriptors, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, D3D12_DESCRIPTOR_HEAP_FLAG_NONE);
		}
	}

	pvt.samplerHeap = new D3D12Core::DescriptorHeap(1024, 0, D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE);

	{
		const uint32_t numRTVDescriptors = 500000u;

		const uint32_t numStaticRTVDescriptors = 150000u;

		pvt.renderTargetHeap = new D3D12Core::DescriptorHeap(numRTVDescriptors, numRTVDescriptors - numStaticRTVDescriptors, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, D3D12_DESCRIPTOR_HEAP_FLAG_NONE);

		pvt.depthStencilHeap = new D3D12Core::DescriptorHeap(numRTVDescriptors, numRTVDescriptors - numStaticRTVDescriptors, D3D12_DESCRIPTOR_HEAP_TYPE_DSV, D3D12_DESCRIPTOR_HEAP_FLAG_NONE);
	}

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

