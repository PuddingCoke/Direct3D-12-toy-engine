#include<Gear/Core/Resource/DynamicCBuffer.h>

#include<Gear/Core/DynamicCBufferManager.h>

#include<Gear/Core/Graphics.h>

#include<Gear/Utils/Logger.h>

#include<Gear/Utils/Math.h>

Gear::Core::Resource::DynamicCBuffer::DynamicCBuffer(const uint32_t size) :
	ImmutableCBuffer(nullptr, 0u, true), regionIndex(Gear::Utils::Math::log2(size / 256u)), dataPtr(nullptr), acquireFrameIndex(UINT64_MAX), updateFrameIndex(UINT64_MAX)
{
	if (regionIndex > 1u)
	{
		LOGERROR(L"size should be one of 256bytes 512bytes");
	}
}

void Gear::Core::Resource::DynamicCBuffer::acquireDataPtr()
{
#ifdef _DEBUG
	if (acquireFrameIndex == Gear::Core::Graphics::getRenderedFrameCount())
	{
		LOGERROR(L"you can only acquire data pointer for cbuffer once per frame");
	}
#endif // _DEBUG

	acquireFrameIndex = Gear::Core::Graphics::getRenderedFrameCount();

	const Gear::Core::DynamicCBufferManager::AvailableLocation& location = Gear::Core::DynamicCBufferManager::requestLocation(regionIndex);

	dataPtr = location.dataPtr;

	gpuAddress = location.gpuAddress;

	bufferIndex = location.descriptorIndex;
}

void Gear::Core::Resource::DynamicCBuffer::updateData(const void* const data)
{
#ifdef _DEBUG
	if (acquireFrameIndex != Gear::Core::Graphics::getRenderedFrameCount())
	{
		LOGERROR(L"you haven't acquire data pointer for this dynamic cbuffer yet");
	}

	if (updateFrameIndex == Gear::Core::Graphics::getRenderedFrameCount())
	{
		LOGERROR(L"you can only update cbuffer once per frame");
	}
#endif // _DEBUG

	updateFrameIndex = Gear::Core::Graphics::getRenderedFrameCount();

	memcpy(dataPtr, data, 256ull << regionIndex);
}

void Gear::Core::Resource::DynamicCBuffer::simpleUpdate(const void* const data)
{
	acquireDataPtr();

	updateData(data);
}
