#include<Gear/Core/Resource/DynamicCBuffer.h>

#include<Gear/Core/DynamicCBufferManager.h>

#include<Gear/Core/Graphics.h>

#include<Gear/Utils/Logger.h>

#include<Gear/Utils/Math.h>

DynamicCBuffer::DynamicCBuffer(const uint32_t size) :
	ImmutableCBuffer(nullptr, 0u, true), regionIndex(Utils::Math::log2(size / 256u)), dataPtr(nullptr), acquireFrameIndex(UINT64_MAX), updateFrameIndex(UINT64_MAX)
{
	if (regionIndex > 1u)
	{
		LOGERROR(L"size should be one of 256bytes 512bytes");
	}
}

void DynamicCBuffer::acquireDataPtr()
{
#ifdef _DEBUG
	if (acquireFrameIndex == Core::Graphics::getRenderedFrameCount())
	{
		LOGERROR(L"you can only acquire data pointer for cbuffer once per frame");
	}
#endif // _DEBUG

	acquireFrameIndex = Core::Graphics::getRenderedFrameCount();

	const Core::DynamicCBufferManager::AvailableLocation& location = Core::DynamicCBufferManager::requestLocation(regionIndex);

	dataPtr = location.dataPtr;

	gpuAddress = location.gpuAddress;

	bufferIndex = location.descriptorIndex;
}

void DynamicCBuffer::updateData(const void* const data)
{
#ifdef _DEBUG
	if (acquireFrameIndex != Core::Graphics::getRenderedFrameCount())
	{
		LOGERROR(L"you haven't acquire data pointer for this dynamic cbuffer yet");
	}

	if (updateFrameIndex == Core::Graphics::getRenderedFrameCount())
	{
		LOGERROR(L"you can only update cbuffer once per frame");
	}
#endif // _DEBUG

	updateFrameIndex = Core::Graphics::getRenderedFrameCount();

	memcpy(dataPtr, data, 256u << regionIndex);
}

void DynamicCBuffer::simpleUpdate(const void* const data)
{
	acquireDataPtr();

	updateData(data);
}
