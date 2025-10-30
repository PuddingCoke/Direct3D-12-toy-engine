#include<Gear/Core/D3D12Core/DescriptorHeap.h>

Gear::Core::D3D12Core::DescriptorHeap::DescriptorHeap(const uint32_t numDescriptors, const uint32_t numDynamicDescriptors, const D3D12_DESCRIPTOR_HEAP_TYPE type, const D3D12_DESCRIPTOR_HEAP_FLAGS flags) :
	numDescriptors(numDescriptors), dynamicIndexStart(numDescriptors - numDynamicDescriptors), type(type),
	incrementSize(GraphicsDevice::get()->GetDescriptorHandleIncrementSize(type)), staticIndex(0u), dynamicIndex(dynamicIndexStart)
{
	D3D12_DESCRIPTOR_HEAP_DESC desc = {};
	desc.NodeMask = 0;
	desc.NumDescriptors = numDescriptors;
	desc.Type = type;
	desc.Flags = flags;

	GraphicsDevice::get()->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&descriptorHeap));

	cpuHandleStart = descriptorHeap->GetCPUDescriptorHandleForHeapStart();

	if (flags == D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE)
	{
		gpuHandleStart = descriptorHeap->GetGPUDescriptorHandleForHeapStart();
	}
	else
	{
		gpuHandleStart = CD3DX12_GPU_DESCRIPTOR_HANDLE();
	}
}

ID3D12DescriptorHeap* Gear::Core::D3D12Core::DescriptorHeap::get() const
{
	return descriptorHeap.Get();
}

Gear::Core::D3D12Core::DescriptorHandle Gear::Core::D3D12Core::DescriptorHeap::allocStaticDescriptor(const uint32_t num)
{
	const uint32_t retIndex = staticIndex.fetch_add(num, std::memory_order_relaxed);

	const CD3DX12_CPU_DESCRIPTOR_HANDLE retCPUHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(cpuHandleStart, retIndex, incrementSize);

	const CD3DX12_GPU_DESCRIPTOR_HANDLE retGPUHandle = CD3DX12_GPU_DESCRIPTOR_HANDLE(gpuHandleStart, retIndex, incrementSize);

	return DescriptorHandle(retCPUHandle, retGPUHandle, retIndex, incrementSize);
}

Gear::Core::D3D12Core::DescriptorHandle Gear::Core::D3D12Core::DescriptorHeap::allocDynamicDescriptor(const uint32_t num)
{
	uint32_t retIndex;

	{
		std::lock_guard<std::mutex> lockGuard(dynamicRegionMutex);

		const uint32_t movedLocation = dynamicIndex + num;

		if (movedLocation > numDescriptors)
		{
			dynamicIndex = dynamicIndexStart;
		}

		retIndex = dynamicIndex;

		dynamicIndex += num;
	}

	CD3DX12_CPU_DESCRIPTOR_HANDLE retCPUHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(cpuHandleStart, retIndex, incrementSize);

	CD3DX12_GPU_DESCRIPTOR_HANDLE retGPUHandle = CD3DX12_GPU_DESCRIPTOR_HANDLE(gpuHandleStart, retIndex, incrementSize);

	return DescriptorHandle(retCPUHandle, retGPUHandle, retIndex, incrementSize);
}
