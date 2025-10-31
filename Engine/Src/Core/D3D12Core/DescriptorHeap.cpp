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
	//这里还能继续优化，可以变成取模版本，于是就不需要mutex
	//但是会导致descriptorHandle需要环绕，一旦需要环绕那么getTransientDescriptorHandle方法就会出问题
	//把非持久性资源的descriptor拷贝ResourceDescriptorHeap中会变麻烦，其实这还涉及到如何让非持久性资源的descriptor被管线使用
	//也许根本就不需要copyDescriptors方法。哎，这里好难呀！暂时还没想到该具体怎么改，头皮发麻了............
	//反正得好好想想，正好写了一个月的代码感觉好累的，我也想休息下了
	uint32_t retIndex;

	{
		std::lock_guard<std::mutex> lockGuard(dynamicRegionMutex);

		if (dynamicIndex + num > numDescriptors)
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
