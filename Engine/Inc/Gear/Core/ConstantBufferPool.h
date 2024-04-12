#pragma once

#ifndef _CONSTANTBUFFERPOOL_H_
#define _CONSTANTBUFFERPOOL_H_

#include<Gear/Core/GlobalDescriptorHeap.h>
#include<Gear/Core/DX/Resource/Buffer.h>
#include<Gear/Core/DX/Resource/UploadHeap.h>

#include<mutex>

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

	static constexpr UINT numConstantBufferPool = 3;

private:

	friend class RenderEngine;

	std::vector<UINT> availableIndices;

	Buffer* buffer;

	UINT baseDescriptorIndex;

	UploadHeap*** uploadHeaps;

	UINT* uploadHeapIndices;

	bool* updateIndicator;

	const UINT subRegionSize;

	const UINT subRegionNum;

	std::mutex availableIndicesLock;

};

#endif // !_CONSTANTBUFFERPOOL_H_
