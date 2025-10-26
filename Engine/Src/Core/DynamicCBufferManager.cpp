#include<Gear/Core/DynamicCBufferManager.h>

#include<Gear/Core/Internal/DynamicCBufferManagerInternal.h>

#include<Gear/Core/Resource/D3D12Resource/UploadHeap.h>

#include<Gear/Core/Graphics.h>

#include<atomic>

#include<Gear/Core/GlobalDescriptorHeap.h>

namespace
{
	class DynamicCBufferRegion
	{
	public:

		DynamicCBufferRegion(const uint64_t subRegionSize, const uint64_t subRegionNum);

		~DynamicCBufferRegion();

		uint32_t update(const void* const data);

		void reset();

		uint64_t getUpdateSize() const;

		ID3D12Resource* getResource() const;

	private:

		const uint64_t subRegionSize;

		UploadHeap** const uploadHeap;

		uint8_t** const dataPtr;

		std::atomic<uint64_t> currentOffset;

	};

	class DynamicCBufferManagerPrivate
	{
	public:

		DynamicCBufferManagerPrivate();

		~DynamicCBufferManagerPrivate();

		Core::DynamicCBufferManager::AvailableDescriptor requestDescriptor(const uint32_t regionIndex, const void* const data);

		void recordCommands(CommandList* const commandList);

	private:

		//define number of each region's subregion
		//256bytes 512bytes 1024bytes .....
		static constexpr uint32_t numSubRegion[] = { 4096,2048 };

		static constexpr uint32_t numRegion = sizeof(numSubRegion) / sizeof(uint32_t);

		Buffer* buffer;

		DynamicCBufferRegion** bufferRegions;

		uint64_t dstOffset[numRegion];

		//constantBufferAddress = baseAddressOffsets[regionIndex] + (256 << regionIndex) * subregionIndex
		uint64_t baseAddressOffsets[numRegion];

		//descriptorIndex = baseDescriptorIndices[regionIndex] + subregionIndex 
		uint32_t baseDescriptorIndices[numRegion];

	}*pvt = nullptr;
}

DynamicCBufferRegion::DynamicCBufferRegion(const uint64_t subRegionSize, const uint64_t subRegionNum) :
	subRegionSize(subRegionSize),
	uploadHeap(new UploadHeap* [Core::Graphics::getFrameBufferCount()]),
	dataPtr(new uint8_t* [Core::Graphics::getFrameBufferCount()]),
	currentOffset(0)
{
	for (uint32_t i = 0; i < Core::Graphics::getFrameBufferCount(); i++)
	{
		uploadHeap[i] = new UploadHeap(subRegionSize * subRegionNum);

		dataPtr[i] = static_cast<uint8_t*>(uploadHeap[i]->map());
	}
}

DynamicCBufferRegion::~DynamicCBufferRegion()
{
	if (uploadHeap)
	{
		for (uint32_t i = 0; i < Core::Graphics::getFrameBufferCount(); i++)
		{
			if (uploadHeap[i])
			{
				uploadHeap[i]->unmap();

				delete uploadHeap[i];
			}
		}

		delete[] uploadHeap;
	}

	if (dataPtr)
	{
		delete[] dataPtr;
	}
}

uint32_t DynamicCBufferRegion::update(const void* const data)
{
	const uint64_t tempOffset = currentOffset.fetch_add(subRegionSize, std::memory_order_relaxed);

	memcpy(dataPtr[Core::Graphics::getFrameIndex()] + tempOffset, data, subRegionSize);

	return static_cast<uint32_t>(tempOffset / subRegionSize);
}

void DynamicCBufferRegion::reset()
{
	currentOffset.store(0ull, std::memory_order_relaxed);
}

uint64_t DynamicCBufferRegion::getUpdateSize() const
{
	return currentOffset.load(std::memory_order_relaxed);
}

ID3D12Resource* DynamicCBufferRegion::getResource() const
{
	return uploadHeap[Core::Graphics::getFrameIndex()]->getResource();
}

DynamicCBufferManagerPrivate::DynamicCBufferManagerPrivate()
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

		LOGSUCCESS(L"create", FloatPrecision(0), requiredSize / 1024.f, L"kbytes constant buffer succeeded");
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

		DescriptorHandle descriptorHandle = Core::GlobalDescriptorHeap::getResourceHeap()->allocStaticDescriptor(requiredDescriptorNum);

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

				Core::GraphicsDevice::get()->CreateConstantBufferView(&desc, descriptorHandle.getCPUHandle());

				descriptorHandle.move();
			}
		}
	}
}

DynamicCBufferManagerPrivate::~DynamicCBufferManagerPrivate()
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

Core::DynamicCBufferManager::AvailableDescriptor DynamicCBufferManagerPrivate::requestDescriptor(const uint32_t regionIndex, const void* const data)
{
	const uint32_t subregionIndex = bufferRegions[regionIndex]->update(data);

	return { baseAddressOffsets[regionIndex] + (256u << regionIndex) * subregionIndex,baseDescriptorIndices[regionIndex] + subregionIndex };
}

void DynamicCBufferManagerPrivate::recordCommands(CommandList* const commandList)
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

Core::DynamicCBufferManager::AvailableDescriptor Core::DynamicCBufferManager::requestDescriptor(const uint32_t regionIndex, const void* const data)
{
	return pvt->requestDescriptor(regionIndex, data);
}

void Core::DynamicCBufferManager::Internal::initialize()
{
	pvt = new DynamicCBufferManagerPrivate();
}

void Core::DynamicCBufferManager::Internal::release()
{
	if (pvt)
	{
		delete pvt;
	}
}

void Core::DynamicCBufferManager::Internal::recordCommands(CommandList* const commandList)
{
	pvt->recordCommands(commandList);
}
