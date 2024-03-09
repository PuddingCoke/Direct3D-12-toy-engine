#include<Gear/Core/ConstantBufferPool.h>

ConstantBufferPool::ConstantBufferPool(const UINT subRegionSize, const UINT subRegionNum) :
	subRegionSize(subRegionSize), subRegionNum(subRegionNum)
{
	uploadHeapIndices = new UINT[subRegionNum];

	updateIndicator = new bool[subRegionNum];

	uploadHeaps = new UploadHeap * *[subRegionNum];

	for (UINT i = 0; i < subRegionNum; i++)
	{
		uploadHeapIndices[i] = 0;

		updateIndicator[i] = false;

		uploadHeaps[i] = new UploadHeap * [Graphics::FrameBufferCount];

		for (UINT j = 0; j < Graphics::FrameBufferCount; j++)
		{
			uploadHeaps[i][j] = new UploadHeap(subRegionSize);
		}
	}

	buffer = new Buffer(subRegionSize * subRegionNum, false, nullptr, nullptr, nullptr, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);

	{
		StaticDescriptorHandle descriptorHandle = GlobalDescriptorHeap::getResourceHeap()->allocStaticDescriptor(subRegionNum);

		baseDescriptorIndex = descriptorHandle.getCurrentIndex();

		for (UINT i = 0; i < subRegionNum; i++)
		{
			D3D12_CONSTANT_BUFFER_VIEW_DESC desc = {};
			desc.BufferLocation = buffer->getGPUAddress() + subRegionSize * i;
			desc.SizeInBytes = subRegionSize;

			GraphicsDevice::get()->CreateConstantBufferView(&desc, descriptorHandle.getCPUHandle());

			availableIndices.push_back(descriptorHandle.getCurrentIndex());

			descriptorHandle.move();
		}
	}

	std::cout << "[class ConstantBufferPool] create constant buffer pool with region size " << subRegionSize << "bytes and region number " << subRegionNum << " succeeded\n";
}

ConstantBufferPool::~ConstantBufferPool()
{
	delete buffer;

	for (UINT i = 0; i < subRegionNum; i++)
	{
		for (UINT j = 0; j < Graphics::FrameBufferCount; j++)
		{
			delete uploadHeaps[i][j];
		}

		delete[] uploadHeaps[i];
	}

	delete[] uploadHeaps;

	delete[] uploadHeapIndices;

	delete[] updateIndicator;
}

ConstantBufferPool::AvailableDescriptor ConstantBufferPool::requestAvailableDescriptor()
{
	std::lock_guard<std::mutex> lockGuard(availableIndicesLock);

	UINT index = availableIndices.back();

	availableIndices.pop_back();

	ConstantBufferPool::AvailableDescriptor descriptor = {};
	descriptor.gpuAddress = buffer->getGPUAddress() + (index - baseDescriptorIndex) * subRegionSize;
	descriptor.descriptorIndex = index;

	return descriptor;
}

void ConstantBufferPool::retrieveUsedDescriptor(const UINT descriptorIndex)
{
	std::lock_guard<std::mutex> lockGuard(availableIndicesLock);

	availableIndices.push_back(descriptorIndex);
}

void ConstantBufferPool::updateSubRegion(const UINT descriptorIndex, const void* const data, const UINT size)
{
	const UINT subRegionIndex = descriptorIndex - baseDescriptorIndex;

	const UINT uploadHeapIndex = uploadHeapIndices[subRegionIndex];

	uploadHeaps[subRegionIndex][uploadHeapIndex]->update(data, size);

	updateIndicator[subRegionIndex] = true;
}

void ConstantBufferPool::recordCommands(ID3D12GraphicsCommandList6* const commandList)
{
	for (UINT subRegionIndex = 0; subRegionIndex < subRegionNum; subRegionIndex++)
	{
		if (updateIndicator[subRegionIndex])
		{
			const UINT64 dstOffset = subRegionIndex * subRegionSize;

			const UINT uploadHeapIndex = uploadHeapIndices[subRegionIndex];

			commandList->CopyBufferRegion(buffer->getResource(), dstOffset, uploadHeaps[subRegionIndex][uploadHeapIndex]->getResource(), 0, subRegionSize);

			uploadHeapIndices[subRegionIndex] = (uploadHeapIndices[subRegionIndex] + 1) % Graphics::FrameBufferCount;

			updateIndicator[subRegionIndex] = false;
		}
	}
}
