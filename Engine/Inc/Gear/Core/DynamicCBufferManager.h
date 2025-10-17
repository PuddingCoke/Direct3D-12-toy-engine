#pragma once

#ifndef _DYNAMICCBUFFERMANAGER_H_
#define _DYNAMICCBUFFERMANAGER_H_

#include<Gear/Core/CommandList.h>

#include<Gear/Core/Resource/D3D12Resource/Buffer.h>

#include"DynamicCBufferRegion.h"

class DynamicCBufferManager
{
public:

	DynamicCBufferManager(const DynamicCBufferManager&) = delete;

	void operator=(const DynamicCBufferManager&) = delete;

	DynamicCBufferManager();

	~DynamicCBufferManager();

	//define number of each region's subregion
	//256bytes 512bytes 1024bytes .....
	static constexpr uint32_t numSubRegion[] = { 4096,2048 };

	static constexpr uint32_t numRegion = sizeof(numSubRegion) / sizeof(uint32_t);

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

	uint64_t dstOffset[numRegion];

	//constantBufferAddress = baseAddressOffsets[regionIndex] + (256 << regionIndex) * subregionIndex
	uint64_t baseAddressOffsets[numRegion];

	//descriptorIndex = baseDescriptorIndices[regionIndex] + subregionIndex 
	uint32_t baseDescriptorIndices[numRegion];

};

#endif // !_DYNAMICCBUFFERMANAGER_H_