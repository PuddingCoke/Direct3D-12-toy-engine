#pragma once

#ifndef _DYNAMICCBUFFERMANAGER_H_
#define _DYNAMICCBUFFERMANAGER_H_

#include<Gear/Core/CommandList.h>

#include<Gear/Core/Resource/D3D12Resource/Buffer.h>

#include"DynamicCBufferRegion.h"

//create a large buffer and divide it into 256bytes 512bytes 1024bytes ...... aligned regions
//1024bytes can hold 16 matrices or 64 float4 this is enough for most case(maybe?)
//and also for each frame we need to allocate an uploadheap
class DynamicCBufferManager
{
public:

	DynamicCBufferManager(const DynamicCBufferManager&) = delete;

	void operator=(const DynamicCBufferManager&) = delete;

	DynamicCBufferManager();

	~DynamicCBufferManager();

	//256bytes 512bytes 1024bytes ......
	static constexpr uint32_t numRegion = 3;

	//define number of each region's subregion
	static constexpr uint32_t numSubRegion[numRegion] = { 1024,512,512 };

	struct AvailableDescriptor
	{
		//provide gpu address to directly bind constant buffer
		const D3D12_GPU_VIRTUAL_ADDRESS gpuAddress;
		//provide index to directly access resource in resource descriptor heap
		const uint32_t descriptorIndex;
	} requestDescriptor(const uint32_t regionIndex, const void* const data);

	static DynamicCBufferManager* get();

private:

	static DynamicCBufferManager* instance;

	friend class RenderEngine;

	void recordCommands(CommandList* const commandList);

	Buffer* buffer;

	DynamicCBufferRegion** bufferRegions;

	//constantBufferAddress = bufferGPULocation + baseAddressOffsets[regionIndex] + (256 << regionIndex) * subregionIndex
	uint32_t baseAddressOffsets[numRegion];

	//descriptorIndex = baseDescriptorIndices[regionIndex] + subregionIndex 
	uint32_t baseDescriptorIndices[numRegion];

};

#endif // !_DYNAMICCBUFFERMANAGER_H_