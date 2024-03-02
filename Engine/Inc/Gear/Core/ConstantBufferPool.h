#pragma once

#ifndef _CONSTANTBUFFERPOOL_H_
#define _CONSTANTBUFFERPOOL_H_

#include<Gear/Core/GlobalDescriptorHeap.h>
#include<Gear/Core/DX/Resource/Buffer.h>
#include<Gear/Core/DX/Resource/UploadHeap.h>

#include<list>

class ConstantBufferPool
{
public:

	ConstantBufferPool(const UINT subRegionSize, const UINT subRegionNum);

	~ConstantBufferPool();

	struct AvailableDescriptor
	{
		D3D12_GPU_VIRTUAL_ADDRESS gpuAddress;
		UINT descriptorIndex;
	} requestAvailableDescriptor();

	void retrieveUsedDescriptor(const UINT descriptorIndex);

	void updateSubRegion(const UINT descriptorIndex, const void* const data, const UINT size);

	void recordCommands(ID3D12GraphicsCommandList6* const commandList);

private:

	friend class RenderEngine;

	std::list<UINT> availableIndices;

	Buffer* buffer;

	UINT baseDescriptorIndex;

	UploadHeap*** uploadHeaps;

	UINT* uploadHeapIndices;

	bool* updateIndicator;

	const UINT subRegionSize;

	const UINT subRegionNum;

};

#endif // !_CONSTANTBUFFERPOOL_H_
