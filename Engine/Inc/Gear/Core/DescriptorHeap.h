#pragma once

#ifndef _DESCRIPTORHEAP_H_
#define _DESCRIPTORHEAP_H_

#include<Gear/Core/GraphicsDevice.h>
#include<Gear/Core/Graphics.h>

#include<mutex>

class DescriptorHeap
{
public:
	
	class StaticDescriptorHandle
	{
	public:

		StaticDescriptorHandle(const CD3DX12_CPU_DESCRIPTOR_HANDLE handle, const DescriptorHeap* const descriptorHeap);

		CD3DX12_CPU_DESCRIPTOR_HANDLE getHandle() const;

		virtual void move();

	protected:

		CD3DX12_CPU_DESCRIPTOR_HANDLE handle;

		const DescriptorHeap* const descriptorHeap;

	};

	class DynamicDescriptorHandle :public StaticDescriptorHandle
	{
	public:

		DynamicDescriptorHandle(const CD3DX12_CPU_DESCRIPTOR_HANDLE handle, const DescriptorHeap* const descriptorHeap);

		void move() override;

	};

	DescriptorHeap(const UINT numDescriptors, const UINT subRegionSize, const D3D12_DESCRIPTOR_HEAP_TYPE type, const D3D12_DESCRIPTOR_HEAP_FLAGS flags);

	UINT getNumDescriptors() const;

	UINT getSubRegionSize() const;

	D3D12_DESCRIPTOR_HEAP_TYPE getDescriptorHeapType() const;

	UINT getIncrementSize() const;

	ID3D12DescriptorHeap* get() const;

	StaticDescriptorHandle allocStaticDescriptor(UINT num);

	DynamicDescriptorHandle allocDynamicDescriptor(UINT num);

private:

	const UINT numDescriptors;

	const UINT subRegionSize;

	const D3D12_DESCRIPTOR_HEAP_TYPE type;

	const UINT incrementSize;

	ComPtr<ID3D12DescriptorHeap> descriptorHeap;

	CD3DX12_CPU_DESCRIPTOR_HANDLE staticDescriptorPointer;

	CD3DX12_CPU_DESCRIPTOR_HANDLE dynamicDescriptorPointer;

	CD3DX12_CPU_DESCRIPTOR_HANDLE staticDescriptorPointerStart;

	CD3DX12_CPU_DESCRIPTOR_HANDLE dynamicDescriptorPointerStart;

	std::mutex staticDescriptorLock;

	std::mutex dynamicDescriptorLock;

};

#endif // !_DESCRIPTORHEAP_H_