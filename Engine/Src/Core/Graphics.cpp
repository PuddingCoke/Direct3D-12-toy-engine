#include<Gear/Core/Graphics.h>

#include<Gear/Core/Internal/GraphicsInternal.h>

namespace
{
	struct GraphicsPrivate
	{

		uint32_t frameBufferCount = 0;

		uint32_t frameIndex = 0;

		uint32_t width = 0;

		uint32_t height = 0;

		uint64_t renderedFrameCount = 0;

		float aspectRatio = 0.f;

		float deltaTime = 0.f;

		float timeElapsed = 0.f;

		D3D12_CPU_DESCRIPTOR_HANDLE backBufferHandle;

		Gear::Core::Resource::ImmutableCBuffer* reservedGlobalCBuffer;

	}pvt;
}

uint32_t Gear::Core::Graphics::getFrameBufferCount()
{
	return pvt.frameBufferCount;
}

uint32_t Gear::Core::Graphics::getFrameIndex()
{
	return pvt.frameIndex;
}

float Gear::Core::Graphics::getDeltaTime()
{
	return pvt.deltaTime;
}

float Gear::Core::Graphics::getTimeElapsed()
{
	return pvt.timeElapsed;
}

uint32_t Gear::Core::Graphics::getWidth()
{
	return pvt.width;
}

uint32_t Gear::Core::Graphics::getHeight()
{
	return pvt.height;
}

float Gear::Core::Graphics::getAspectRatio()
{
	return pvt.aspectRatio;
}

uint64_t Gear::Core::Graphics::getRenderedFrameCount()
{
	return pvt.renderedFrameCount;
}

D3D12_CPU_DESCRIPTOR_HANDLE Gear::Core::Graphics::getBackBufferHandle()
{
	return pvt.backBufferHandle;
}

Gear::Core::Resource::ImmutableCBuffer* Gear::Core::Graphics::getReservedGlobalCBuffer()
{
	return pvt.reservedGlobalCBuffer;
}

void Gear::Core::Graphics::Internal::initialize(const uint32_t frameBufferCount, const uint32_t width, const uint32_t height)
{
	pvt.frameBufferCount = frameBufferCount;

	pvt.width = width;

	pvt.height = height;

	pvt.aspectRatio = static_cast<float>(width) / static_cast<float>(height);
}

void Gear::Core::Graphics::Internal::renderedFrameCountInc()
{
	pvt.renderedFrameCount++;
}

void Gear::Core::Graphics::Internal::setFrameIndex(const uint32_t frameIndex)
{
	pvt.frameIndex = frameIndex;
}

void Gear::Core::Graphics::Internal::setDeltaTime(const float deltaTime)
{
	pvt.deltaTime = deltaTime;
}

void Gear::Core::Graphics::Internal::updateTimeElapsed()
{
	pvt.timeElapsed += pvt.deltaTime;
}

void Gear::Core::Graphics::Internal::setBackBufferHandle(const D3D12_CPU_DESCRIPTOR_HANDLE backBufferHandle)
{
	pvt.backBufferHandle = backBufferHandle;
}

void Gear::Core::Graphics::Internal::setReservedGlobalCBuffer(Resource::ImmutableCBuffer* const reservedGlobalCBuffer)
{
	pvt.reservedGlobalCBuffer = reservedGlobalCBuffer;
}
