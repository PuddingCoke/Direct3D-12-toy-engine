#pragma once

#ifndef _DESCRIPTORHEAP_H_
#define _DESCRIPTORHEAP_H_

#include<Gear/Core/GraphicsDevice.h>

#include<mutex>

class DescriptorHandle;

class DescriptorHeap
{
public:

	DescriptorHeap(const uint32_t numDescriptors, const uint32_t subRegionSize, const D3D12_DESCRIPTOR_HEAP_TYPE type, const D3D12_DESCRIPTOR_HEAP_FLAGS flags);

	uint32_t getNumDescriptors() const;

	uint32_t getSubRegionSize() const;

	D3D12_DESCRIPTOR_HEAP_TYPE getDescriptorHeapType() const;

	uint32_t getIncrementSize() const;

	ID3D12DescriptorHeap* get() const;

	DescriptorHandle allocStaticDescriptor(const uint32_t num);

	DescriptorHandle allocDynamicDescriptor(const uint32_t num);

private:

	friend class DescriptorHandle;

	const uint32_t numDescriptors;

	const uint32_t subRegionSize;

	const D3D12_DESCRIPTOR_HEAP_TYPE type;

	const uint32_t incrementSize;

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

class DescriptorHandle
{
public:

	DescriptorHandle();

	DescriptorHandle(const CD3DX12_CPU_DESCRIPTOR_HANDLE cpuHandle, const CD3DX12_GPU_DESCRIPTOR_HANDLE gpuHandle, const DescriptorHeap* const descriptorHeap);

	uint32_t getCurrentIndex() const;

	CD3DX12_CPU_DESCRIPTOR_HANDLE getCPUHandle() const;

	CD3DX12_GPU_DESCRIPTOR_HANDLE getGPUHandle() const;

	void move();

	void offset(const uint32_t num);

protected:

	CD3DX12_CPU_DESCRIPTOR_HANDLE cpuHandle;

	CD3DX12_GPU_DESCRIPTOR_HANDLE gpuHandle;

	const DescriptorHeap* descriptorHeap;

};

#endif // !_DESCRIPTORHEAP_H_