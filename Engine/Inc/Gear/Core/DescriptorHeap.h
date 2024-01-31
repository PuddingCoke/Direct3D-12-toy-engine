#pragma once

#ifndef _DESCRIPTORHEAP_H_
#define _DESCRIPTORHEAP_H_

#include<Gear/Core/GraphicsDevice.h>

class DescriptorHeap
{
public:

	DescriptorHeap(const UINT numDescriptors, const D3D12_DESCRIPTOR_HEAP_TYPE type, const D3D12_DESCRIPTOR_HEAP_FLAGS flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE);

	UINT getNumDescriptors();

	D3D12_DESCRIPTOR_HEAP_TYPE getDescriptorHeapType();

	void get();

private:

	const UINT numDescriptors;

	const D3D12_DESCRIPTOR_HEAP_TYPE type;

	ComPtr<ID3D12DescriptorHeap> descriptorHeap;

};

#endif // !_DESCRIPTORHEAP_H_
