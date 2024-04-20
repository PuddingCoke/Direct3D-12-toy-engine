#pragma once

#ifndef _DESCRIPTORHEAP_H_
#define _DESCRIPTORHEAP_H_

#include<Gear/Core/GraphicsDevice.h>
#include<Gear/Core/Graphics.h>

#include<mutex>

class DescriptorHeap
{
public:
	
	class DescriptorHandle
	{
	public:

		DescriptorHandle();

		DescriptorHandle(const CD3DX12_CPU_DESCRIPTOR_HANDLE cpuHandle, const CD3DX12_GPU_DESCRIPTOR_HANDLE gpuHandle, const DescriptorHeap* const descriptorHeap);

		UINT getCurrentIndex() const;

		CD3DX12_CPU_DESCRIPTOR_HANDLE getCPUHandle() const;

		CD3DX12_GPU_DESCRIPTOR_HANDLE getGPUHandle() const;

		void move();

	protected:

		CD3DX12_CPU_DESCRIPTOR_HANDLE cpuHandle;

		CD3DX12_GPU_DESCRIPTOR_HANDLE gpuHandle;

		const DescriptorHeap* descriptorHeap;

	};

	DescriptorHeap(const UINT numDescriptors, const UINT subRegionSize, const D3D12_DESCRIPTOR_HEAP_TYPE type, const D3D12_DESCRIPTOR_HEAP_FLAGS flags);

	UINT getNumDescriptors() const;

	UINT getSubRegionSize() const;

	D3D12_DESCRIPTOR_HEAP_TYPE getDescriptorHeapType() const;

	UINT getIncrementSize() const;

	ID3D12DescriptorHeap* get() const;

	DescriptorHandle allocStaticDescriptor(const UINT num);

	DescriptorHandle allocDynamicDescriptor(const UINT num);

private:

	const UINT numDescriptors;

	const UINT subRegionSize;

	const D3D12_DESCRIPTOR_HEAP_TYPE type;

	const UINT incrementSize;

	ComPtr<ID3D12DescriptorHeap> descriptorHeap;

	CD3DX12_CPU_DESCRIPTOR_HANDLE staticCPUPointer;

	CD3DX12_CPU_DESCRIPTOR_HANDLE dynamicCPUPointer;

	CD3DX12_CPU_DESCRIPTOR_HANDLE staticCPUPointerStart;

	CD3DX12_CPU_DESCRIPTOR_HANDLE dynamicCPUPointerStart;

	CD3DX12_GPU_DESCRIPTOR_HANDLE staticGPUPointer;

	CD3DX12_GPU_DESCRIPTOR_HANDLE dynamicGPUPointer;

	CD3DX12_GPU_DESCRIPTOR_HANDLE staticGPUPointerStart;

	CD3DX12_GPU_DESCRIPTOR_HANDLE dynamicGPUPointerStart;

	std::mutex staticPointerLock;

	std::mutex dynamicPointerLock;

};

#endif // !_DESCRIPTORHEAP_H_
