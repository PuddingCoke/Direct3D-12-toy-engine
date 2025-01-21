#include<Gear/Core/ConstantBufferManager.h>

ConstantBufferManager* ConstantBufferManager::instance = nullptr;

ConstantBufferManager::ConstantBufferManager() :
	updateUploadHeapIndex(0)
{
	//create a large buffer that used as constant buffer
	{
		uint32_t requiredSize = 0;

		for (uint32_t regionIndex = 0; regionIndex < numRegion; regionIndex++)
		{
			requiredSize += (256 << regionIndex) * numSubRegion[regionIndex];
		}

		buffer = new Buffer(requiredSize, true, D3D12_RESOURCE_FLAG_NONE);

		buffer->setName(L"Large Constant Buffer");

		LOGSUCCESS("create", requiredSize / 1024.f, "kbytes constant buffer succeeded");
	}

	//create all neccessary containers
	for (uint32_t regionIndex = 0; regionIndex < numRegion; regionIndex++)
	{
		uploadHeaps[regionIndex] = std::vector<std::vector<UploadHeap*>>(numSubRegion[regionIndex]);

		updateIndicators[regionIndex] = std::vector<bool>(numSubRegion[regionIndex], false);

		for (uint32_t subregionIndex = 0; subregionIndex < numSubRegion[regionIndex]; subregionIndex++)
		{
			//every frame need a upload heap since we are using triple buffering
			uploadHeaps[regionIndex][subregionIndex] = std::vector<UploadHeap*>(Graphics::getFrameBufferCount());

			for (uint32_t uploadHeapIndex = 0; uploadHeapIndex < Graphics::getFrameBufferCount(); uploadHeapIndex++)
			{
				uploadHeaps[regionIndex][subregionIndex][uploadHeapIndex] = new UploadHeap((256 << regionIndex));

				uploadHeaps[regionIndex][subregionIndex][uploadHeapIndex]->getResource()->SetName(L"Constant Buffer Upload Heap");
			}
		}

		availableDescriptorIndices[regionIndex] = std::vector<uint32_t>(numSubRegion[regionIndex]);
	}

	//create descriptors
	{
		uint32_t requiredDescriptorNum = 0;

		{
			for (uint32_t regionIndex = 0; regionIndex < numRegion; regionIndex++)
			{
				requiredDescriptorNum += numSubRegion[regionIndex];
			}
		}

		DescriptorHandle descriptorHandle = GlobalDescriptorHeap::getResourceHeap()->allocStaticDescriptor(requiredDescriptorNum);

		uint32_t bufferLocationOffset = 0;

		for (uint32_t regionIndex = 0; regionIndex < numRegion; regionIndex++)
		{
			baseAddressOffsets[regionIndex] = bufferLocationOffset;

			baseDescriptorIndices[regionIndex] = descriptorHandle.getCurrentIndex();

			for (uint32_t subregionIndex = 0; subregionIndex < numSubRegion[regionIndex]; subregionIndex++)
			{
				D3D12_CONSTANT_BUFFER_VIEW_DESC desc = {};
				desc.BufferLocation = buffer->getGPUAddress() + bufferLocationOffset;
				desc.SizeInBytes = (256 << regionIndex);

				bufferLocationOffset += (256 << regionIndex);

				GraphicsDevice::get()->CreateConstantBufferView(&desc, descriptorHandle.getCPUHandle());

				availableDescriptorIndices[regionIndex][subregionIndex] = descriptorHandle.getCurrentIndex();

				descriptorHandle.move();
			}
		}
	}
}

ConstantBufferManager::~ConstantBufferManager()
{
	delete buffer;

	for (uint32_t regionIndex = 0; regionIndex < numRegion; regionIndex++)
	{
		for (uint32_t subregionIndex = 0; subregionIndex < numSubRegion[regionIndex]; subregionIndex++)
		{
			for (uint32_t uploadHeapIndex = 0; uploadHeapIndex < Graphics::getFrameBufferCount(); uploadHeapIndex++)
			{
				delete uploadHeaps[regionIndex][subregionIndex][uploadHeapIndex];
			}
		}
	}
}

ConstantBufferManager::AvailableDescriptor ConstantBufferManager::requestDescriptor(const uint32_t regionIndex)
{
	uint32_t descriptorIndex;

	{
		std::lock_guard<std::mutex> lockGuard(mutexes[regionIndex]);

		descriptorIndex = availableDescriptorIndices[regionIndex].back();

		availableDescriptorIndices[regionIndex].pop_back();
	}

	const ConstantBufferManager::AvailableDescriptor descriptor = {
		buffer->getGPUAddress() + baseAddressOffsets[regionIndex] + (256 << regionIndex) * (descriptorIndex - baseDescriptorIndices[regionIndex]),
		descriptorIndex
	};

	return descriptor;
}

void ConstantBufferManager::retrieveDescriptor(const uint32_t regionIndex, const uint32_t descriptorIndex)
{
	std::lock_guard<std::mutex> lockGuard(mutexes[regionIndex]);

	availableDescriptorIndices[regionIndex].push_back(descriptorIndex);
}

void ConstantBufferManager::updateSubregion(const uint32_t regionIndex, const uint32_t descriptorIndex, const void* const data, const uint32_t size)
{
	const uint32_t subregionIndex = descriptorIndex - baseDescriptorIndices[regionIndex];

	if (updateIndicators[regionIndex][subregionIndex])
	{
		LOGERROR("a constant buffer in a frame can't be updated multiple times");
	}

	updateIndicators[regionIndex][subregionIndex] = true;

	uploadHeaps[regionIndex][subregionIndex][updateUploadHeapIndex]->update(data, size);

	updateSubregionIndices[regionIndex].push_back(subregionIndex);
}

ConstantBufferManager* ConstantBufferManager::get()
{
	return instance;
}

void ConstantBufferManager::recordCommands(CommandList* const commandList)
{
	commandList->pushResourceTrackList(buffer);

	buffer->setState(D3D12_RESOURCE_STATE_COPY_DEST);

	commandList->transitionResources();

	for (uint32_t regionIndex = 0; regionIndex < numRegion; regionIndex++)
	{
		for (const uint32_t subregionIndex : updateSubregionIndices[regionIndex])
		{
			const uint64_t dstOffset = baseAddressOffsets[regionIndex] + (256 << regionIndex) * subregionIndex;

			updateIndicators[regionIndex][subregionIndex] = false;

			commandList->get()->CopyBufferRegion(buffer->getResource(), dstOffset, uploadHeaps[regionIndex][subregionIndex][updateUploadHeapIndex]->getResource(), 0, (256 << regionIndex));
		}

		updateSubregionIndices[regionIndex].clear();
	}

	updateUploadHeapIndex = (updateUploadHeapIndex + 1) % Graphics::getFrameBufferCount();

	commandList->pushResourceTrackList(buffer);

	buffer->setState(D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);

	commandList->transitionResources();
}
