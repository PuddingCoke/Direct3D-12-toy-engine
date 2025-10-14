#include<Gear/Core/DynamicCBufferManager.h>

#include<Gear/Core/GlobalDescriptorHeap.h>

#include<Gear/Core/Graphics.h>

DynamicCBufferManager* DynamicCBufferManager::instance = nullptr;

DynamicCBufferManager::DynamicCBufferManager()
{
	//create a large buffer that used as constant buffer
	{
		uint32_t requiredSize = 0;

		for (uint32_t regionIndex = 0; regionIndex < numRegion; regionIndex++)
		{
			dstOffset[regionIndex] = requiredSize;

			requiredSize += (256u << regionIndex) * numSubRegion[regionIndex];
		}

		buffer = new Buffer(requiredSize, true, D3D12_RESOURCE_FLAG_NONE);

		buffer->setName(L"Large Constant Buffer");

		LOGSUCCESS(L"create", requiredSize / 1024.f, L"kbytes constant buffer succeeded");
	}

	//create large upload heap for each region
	{
		bufferRegions = new DynamicCBufferRegion * [numRegion];

		for (uint32_t regionIndex = 0; regionIndex < numRegion; regionIndex++)
		{
			bufferRegions[regionIndex] = new DynamicCBufferRegion(256u << regionIndex, numSubRegion[regionIndex]);
		}
	}

	//create descriptors for each region's subregion
	{
		uint32_t requiredDescriptorNum = 0;

		for (uint32_t regionIndex = 0; regionIndex < numRegion; regionIndex++)
		{
			requiredDescriptorNum += numSubRegion[regionIndex];
		}

		DescriptorHandle descriptorHandle = GlobalDescriptorHeap::getResourceHeap()->allocStaticDescriptor(requiredDescriptorNum);

		uint64_t bufferLocationOffset = buffer->getGPUAddress();

		for (uint32_t regionIndex = 0; regionIndex < numRegion; regionIndex++)
		{
			baseAddressOffsets[regionIndex] = bufferLocationOffset;

			baseDescriptorIndices[regionIndex] = descriptorHandle.getCurrentIndex();

			for (uint32_t subregionIndex = 0; subregionIndex < numSubRegion[regionIndex]; subregionIndex++)
			{
				D3D12_CONSTANT_BUFFER_VIEW_DESC desc = {};
				desc.BufferLocation = bufferLocationOffset;
				desc.SizeInBytes = (256u << regionIndex);

				bufferLocationOffset += (256u << regionIndex);

				GraphicsDevice::get()->CreateConstantBufferView(&desc, descriptorHandle.getCPUHandle());

				descriptorHandle.move();
			}
		}
	}
}

DynamicCBufferManager::~DynamicCBufferManager()
{
	if (buffer)
	{
		delete buffer;
	}

	if (bufferRegions)
	{
		for (uint32_t i = 0; i < numRegion; i++)
		{
			if (bufferRegions[i])
			{
				delete bufferRegions[i];
			}
		}

		delete[] bufferRegions;
	}
}

DynamicCBufferManager::AvailableDescriptor DynamicCBufferManager::requestDescriptor(const uint32_t regionIndex, const void* const data)
{
	const uint32_t subregionIndex = bufferRegions[regionIndex]->update(data);

	const DynamicCBufferManager::AvailableDescriptor descriptor = {
		baseAddressOffsets[regionIndex] + (256u << regionIndex) * subregionIndex,
		baseDescriptorIndices[regionIndex] + subregionIndex
	};

	return descriptor;
}

DynamicCBufferManager* DynamicCBufferManager::get()
{
	return instance;
}

void DynamicCBufferManager::recordCommands(CommandList* const commandList)
{
	commandList->pushResourceTrackList(buffer);

	buffer->setState(D3D12_RESOURCE_STATE_COPY_DEST);

	commandList->transitionResources();

	for (uint32_t regionIndex = 0; regionIndex < numRegion; regionIndex++)
	{
		if (bufferRegions[regionIndex]->getUpdateSize())
		{
			commandList->get()->CopyBufferRegion(buffer->getResource(), dstOffset[regionIndex],
				bufferRegions[regionIndex]->getResource(), 0,
				bufferRegions[regionIndex]->getUpdateSize());

			bufferRegions[regionIndex]->reset();
		}
	}

	commandList->pushResourceTrackList(buffer);

	buffer->setState(D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);

	commandList->transitionResources();
}
