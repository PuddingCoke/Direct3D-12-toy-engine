#include<Gear/Core/ConstantBufferManager.h>

ConstantBufferManager* ConstantBufferManager::instance = nullptr;

ConstantBufferManager::ConstantBufferManager() :
	updateUploadHeapIndex(0)
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

		std::cout << "[class ConstantBufferManager] create " << requiredSize / 1024.f << "kbytes constant buffer\n";
	}

	//create all neccessary containers
	for (UINT regionIndex = 0; regionIndex < numRegion; regionIndex++)
	{
		uploadHeaps[regionIndex] = std::vector<std::vector<UploadHeap*>>(numSubRegion[regionIndex]);

		updateIndicators[regionIndex] = std::vector<bool>(numSubRegion[regionIndex], false);

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

		availableDescriptorIndices[regionIndex] = std::vector<UINT>(numSubRegion[regionIndex]);
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

				availableDescriptorIndices[regionIndex][subregionIndex] = descriptorHandle.getCurrentIndex();

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
	UINT descriptorIndex;

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

void ConstantBufferManager::retrieveDescriptor(const UINT regionIndex, const UINT descriptorIndex)
{
	std::lock_guard<std::mutex> lockGuard(mutexes[regionIndex]);

	availableDescriptorIndices[regionIndex].push_back(descriptorIndex);
}

void ConstantBufferManager::updateSubregion(const UINT regionIndex, const UINT descriptorIndex, const void* const data, const UINT size)
{
	const UINT subregionIndex = descriptorIndex - baseDescriptorIndices[regionIndex];

	if (updateIndicators[regionIndex][subregionIndex])
	{
		throw "you cannot update a constant buffer in a frame multiple time";
	}

	updateIndicators[regionIndex][subregionIndex] = true;

	uploadHeaps[regionIndex][subregionIndex][updateUploadHeapIndex]->update(data, size);

	updateSubregionIndices[regionIndex].push_back(subregionIndex);
}

ConstantBufferManager* ConstantBufferManager::get()
{
	return instance;
}

void ConstantBufferManager::recordCommands(ID3D12GraphicsCommandList6* const commandList)
{
	for (UINT regionIndex = 0; regionIndex < numRegion; regionIndex++)
	{
		for (const UINT subregionIndex : updateSubregionIndices[regionIndex])
		{
			const UINT64 dstOffset = baseAddressOffsets[regionIndex] + (256 << regionIndex) * subregionIndex;

			updateIndicators[regionIndex][subregionIndex] = false;

			commandList->CopyBufferRegion(buffer->getResource(), dstOffset, uploadHeaps[regionIndex][subregionIndex][updateUploadHeapIndex]->getResource(), 0, (256 << regionIndex));
		}

		updateSubregionIndices[regionIndex].clear();
	}

	updateUploadHeapIndex = (updateUploadHeapIndex + 1) % Graphics::FrameBufferCount;
}
