#include<Gear/Core/Resource/DynamicCBuffer.h>

#include<Gear/Core/DynamicCBufferManager.h>

#include<Gear/Core/Graphics.h>

#include<Gear/Utils/Logger.h>

DynamicCBuffer::DynamicCBuffer(const uint32_t size) :
	ImmutableCBuffer(nullptr, 0u, true), updateFrameIndex(UINT64_MAX)
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
#ifdef _DEBUG
	if (updateFrameIndex == Graphics::getRenderedFrameCount())
	{
		LOGERROR(L"cannot update dynamic constant buffer multiple time during one frame");
	}
#endif // _DEBUG

	updateFrameIndex = Graphics::getRenderedFrameCount();

	const DynamicCBufferManager::AvailableDescriptor& descriptor = DynamicCBufferManager::get()->requestDescriptor(regionIndex, data);

	gpuAddress = descriptor.gpuAddress;

	bufferIndex = descriptor.descriptorIndex;
}
