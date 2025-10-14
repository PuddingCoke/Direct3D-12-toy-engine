#include<Gear/Core/Resource/DynamicCBuffer.h>

#include<Gear/Core/DynamicCBufferManager.h>

#include<Gear/Utils/Logger.h>

DynamicCBuffer::DynamicCBuffer(const uint32_t size) :
	ImmutableCBuffer(nullptr, 0u, true)
{
	switch (size)
	{
	case 256:
		regionIndex = 0;
		break;
	case 512:
		regionIndex = 1;
		break;
	default:
		LOGERROR(L"size should be one of 256 512");
		break;
	}
}

void DynamicCBuffer::update(const void* const data)
{
	const DynamicCBufferManager::AvailableDescriptor& descriptor = DynamicCBufferManager::get()->requestDescriptor(regionIndex, data);

	gpuAddress = descriptor.gpuAddress;

	bufferIndex = descriptor.descriptorIndex;
}
