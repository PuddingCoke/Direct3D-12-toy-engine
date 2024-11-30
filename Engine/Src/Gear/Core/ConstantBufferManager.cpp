#include<Gear/Core/ConstantBufferManager.h>

ConstantBufferManager* ConstantBufferManager::instance = nullptr;

ConstantBufferManager::ConstantBufferManager()
{
	//create a large buffer that used as constant buffer
	{
		UINT requiredSize = 0;

		for (UINT regionIndex = 0; regionIndex < numRegion; regionIndex++)
		{
			requiredSize += (256 << regionIndex) * numSubRegion[regionIndex];
		}

		buffer = new Buffer(requiredSize, true, D3D12_RESOURCE_FLAG_NONE);

		buffer->setName(L"Large Constant Buffer");

		std::cout << "[class ConstantBufferManager] create " << requiredSize << "bytes constant buffer\n";
	}

	//create all neccessary containers
	for (UINT regionIndex = 0; regionIndex < numRegion; regionIndex++)
	{
		uploadHeaps[regionIndex] = std::vector<std::vector<UploadHeap*>>(numSubRegion[regionIndex]);

		for (UINT subregionIndex = 0; subregionIndex < numSubRegion[regionIndex]; subregionIndex++)
		{
			//every frame need a upload heap since we are using triple buffering
			uploadHeaps[regionIndex][subregionIndex] = std::vector<UploadHeap*>(Graphics::FrameBufferCount);

			for (UINT uploadHeapIndex = 0; uploadHeapIndex < Graphics::FrameBufferCount; uploadHeapIndex++)
			{
				uploadHeaps[regionIndex][subregionIndex][uploadHeapIndex] = new UploadHeap((256 << regionIndex));

				uploadHeaps[regionIndex][subregionIndex][uploadHeapIndex]->getResource()->SetName(L"Constant Buffer Upload Heap");
			}
		}

		availableIndices[regionIndex] = std::vector<UINT>(numSubRegion[regionIndex]);

		updateUploadHeapIndices[regionIndex] = std::vector<UINT>(numSubRegion[regionIndex], 0);
	}

	//create descriptors
	{
		UINT requiredDescriptorNum = 0;

		{
			for (UINT regionIndex = 0; regionIndex < numRegion; regionIndex++)
			{
				requiredDescriptorNum += numSubRegion[regionIndex];
			}
		}

		DescriptorHandle descriptorHandle = GlobalDescriptorHeap::getResourceHeap()->allocStaticDescriptor(requiredDescriptorNum);

		UINT bufferLocationOffset = 0;

		for (UINT regionIndex = 0; regionIndex < numRegion; regionIndex++)
		{
			baseAddressOffsets[regionIndex] = bufferLocationOffset;

			baseDescriptorIndices[regionIndex] = descriptorHandle.getCurrentIndex();

			for (UINT subregionIndex = 0; subregionIndex < numSubRegion[regionIndex]; subregionIndex++)
			{
				D3D12_CONSTANT_BUFFER_VIEW_DESC desc = {};
				desc.BufferLocation = buffer->getGPUAddress() + bufferLocationOffset;
				desc.SizeInBytes = (256 << regionIndex);

				bufferLocationOffset += (256 << regionIndex);

				GraphicsDevice::get()->CreateConstantBufferView(&desc, descriptorHandle.getCPUHandle());

				availableIndices[regionIndex][subregionIndex] = descriptorHandle.getCurrentIndex();

				descriptorHandle.move();
			}
		}
	}
}

ConstantBufferManager::~ConstantBufferManager()
{
	delete buffer;

	for (UINT regionIndex = 0; regionIndex < numRegion; regionIndex++)
	{
		for (UINT subregionIndex = 0; subregionIndex < numSubRegion[regionIndex]; subregionIndex++)
		{
			for (UINT uploadHeapIndex = 0; uploadHeapIndex < Graphics::FrameBufferCount; uploadHeapIndex++)
			{
				delete uploadHeaps[regionIndex][subregionIndex][uploadHeapIndex];
			}
		}
	}
}

ConstantBufferManager::AvailableDescriptor ConstantBufferManager::requestDescriptor(const UINT regionIndex)
{
	UINT index;

	{
		std::lock_guard<std::mutex> lockGuard(mutexes[regionIndex]);

		index = availableIndices[regionIndex].back();

		availableIndices[regionIndex].pop_back();
	}

	const ConstantBufferManager::AvailableDescriptor descriptor = {
		buffer->getGPUAddress() + baseAddressOffsets[regionIndex] + (256 << regionIndex) * (index - baseDescriptorIndices[regionIndex]),
		index
	};

	return descriptor;
}

void ConstantBufferManager::retrieveDescriptor(const UINT regionIndex, const UINT descriptorIndex)
{
	std::lock_guard<std::mutex> lockGuard(mutexes[regionIndex]);

	availableIndices[regionIndex].push_back(descriptorIndex);
}

void ConstantBufferManager::updateSubregion(const UINT regionIndex, const UINT descriptorIndex, const void* const data, const UINT size)
{
	const UINT subregionIndex = descriptorIndex - baseDescriptorIndices[regionIndex];

	const UINT uploadHeapIndex = updateUploadHeapIndices[regionIndex][subregionIndex];

	uploadHeaps[regionIndex][subregionIndex][uploadHeapIndex]->update(data, size);

	updateIndices[regionIndex].push_back(subregionIndex);
}

ConstantBufferManager* ConstantBufferManager::get()
{
	return instance;
}

void ConstantBufferManager::recordCommands(ID3D12GraphicsCommandList6* const commandList)
{
	for (UINT regionIndex = 0; regionIndex < numRegion; regionIndex++)
	{
		for (const UINT subregionIndex : updateIndices[regionIndex])
		{
			const UINT64 dstOffset = baseAddressOffsets[regionIndex] + (256 << regionIndex) * subregionIndex;

			const UINT uploadHeapIndex = updateUploadHeapIndices[regionIndex][subregionIndex];

			commandList->CopyBufferRegion(buffer->getResource(), dstOffset, uploadHeaps[regionIndex][subregionIndex][uploadHeapIndex]->getResource(), 0, (256 << regionIndex));

			updateUploadHeapIndices[regionIndex][subregionIndex] = (updateUploadHeapIndices[regionIndex][subregionIndex] + 1) % Graphics::FrameBufferCount;
		}

		updateIndices[regionIndex].clear();
	}
}
