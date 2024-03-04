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

		uploadHeaps[i][0] = new UploadHeap(subRegionSize);

		uploadHeaps[i][1] = new UploadHeap(subRegionSize);

		uploadHeaps[i][2] = new UploadHeap(subRegionSize);
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
		delete uploadHeaps[i][0];
		delete uploadHeaps[i][1];
		delete uploadHeaps[i][2];

		delete[] uploadHeaps[i];
	}

	delete[] uploadHeaps;

	delete[] uploadHeapIndices;

	delete[] updateIndicator;
}

ConstantBufferPool::AvailableDescriptor ConstantBufferPool::requestAvailableDescriptor()
{
	UINT index = availableIndices.back();

	availableIndices.pop_back();

	ConstantBufferPool::AvailableDescriptor descriptor = {};
	descriptor.gpuAddress = buffer->getGPUAddress() + (index - baseDescriptorIndex) * subRegionSize;
	descriptor.descriptorIndex = index;

	return descriptor;
}

void ConstantBufferPool::retrieveUsedDescriptor(const UINT descriptorIndex)
{
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
