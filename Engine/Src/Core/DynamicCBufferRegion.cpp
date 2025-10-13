#include<Gear/Core/DynamicCBufferRegion.h>

#include<Gear/Core/Graphics.h>

DynamicCBufferRegion::DynamicCBufferRegion(const uint64_t subRegionSize, const uint64_t subRegionNum) :
	subRegionSize(subRegionSize), currentOffset(0)
{
	uploadHeap = new UploadHeap * [Graphics::getFrameBufferCount()];

	dataPtr = new uint8_t * [Graphics::getFrameBufferCount()];

	for (uint32_t i = 0; i < Graphics::getFrameBufferCount(); i++)
	{
		uploadHeap[i] = new UploadHeap(subRegionSize * subRegionNum);

		dataPtr[i] = static_cast<uint8_t*>(uploadHeap[i]->map());
	}
}

DynamicCBufferRegion::~DynamicCBufferRegion()
{
	if (uploadHeap)
	{
		for (uint32_t i = 0; i < Graphics::getFrameBufferCount(); i++)
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
	uint64_t tempOffset;

	{
		std::lock_guard<std::mutex> lockGuard(offsetMutex);

		tempOffset = currentOffset;

		currentOffset += subRegionSize;
	}

	memcpy(dataPtr[Graphics::getFrameIndex()] + tempOffset, data, subRegionSize);

	return static_cast<uint32_t>(tempOffset / subRegionSize);
}

void DynamicCBufferRegion::reset()
{
	currentOffset = 0;
}

ID3D12Resource* DynamicCBufferRegion::getResource() const
{
	return uploadHeap[Graphics::getFrameIndex()]->getResource();
}

uint64_t DynamicCBufferRegion::getUpdateSize() const
{
	return currentOffset;
}
