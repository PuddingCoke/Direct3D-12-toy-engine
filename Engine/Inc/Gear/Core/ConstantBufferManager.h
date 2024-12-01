#pragma once

#ifndef _CONSTANTBUFFERMANAGER_H_
#define _CONSTANTBUFFERMANAGER_H_

#include<Gear/Core/CommandList.h>
#include<Gear/Core/GlobalDescriptorHeap.h>
#include<Gear/Core/DX/Buffer.h>
#include<Gear/Core/DX/UploadHeap.h>

//create a large buffer and divide it into 256bytes 512bytes 1024bytes ...... aligned regions
//1024bytes can hold 16 matrices or 64 float4 this is enough for most case(maybe?)
//and also for each frame we need to allocate an uploadheap to update corresponding region
class ConstantBufferManager
{
public:

	ConstantBufferManager();

	~ConstantBufferManager();

	//256bytes 512bytes 1024bytes ......
	static constexpr UINT numRegion = 3;

	//define number of each region's subregion
	static constexpr UINT numSubRegion[numRegion] = { 1024,1024,1024 };

	struct AvailableDescriptor
	{
		//provide gpu address to directly bind constant buffer
		const D3D12_GPU_VIRTUAL_ADDRESS gpuAddress;
		//provide index to directly access resource in resource descriptor heap
		const UINT descriptorIndex;
	} requestDescriptor(const UINT regionIndex);

	void retrieveDescriptor(const UINT regionIndex, const UINT descriptorIndex);

	void updateSubregion(const UINT regionIndex, const UINT descriptorIndex, const void* const data, const UINT size);

	static ConstantBufferManager* get();

private:

	static ConstantBufferManager* instance;

	friend class RenderEngine;

	void recordCommands(CommandList* const commandList);

	Buffer* buffer;

	//bufferGPULocation + baseAddressOffset + (256 << regionIndex) * subregionIndex = constantBufferAddress
	UINT baseAddressOffsets[numRegion];

	//descriptorIndex - baseDescriptorIndex = subregionIndex
	UINT baseDescriptorIndices[numRegion];

	//for each frame and each subregion we need [FrameBufferCount] uploadheaps
	std::vector<std::vector<UploadHeap*>> uploadHeaps[numRegion];

	//indicate which subregion need update
	std::vector<bool> updateIndicators[numRegion];

	//store available descriptor indices
	//we use mutex to protect this container
	std::vector<UINT> availableDescriptorIndices[numRegion];

	//prevent race condition
	std::mutex mutexes[numRegion];

	//record subregionIndex of each region that need update
	std::vector<UINT> updateSubregionIndices[numRegion];

	//indicate which uploadheap of subregion need update
	UINT updateUploadHeapIndex;

};

#endif // !_CONSTANTBUFFERMANAGER_H_
