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

		Core::Resource::ImmutableCBuffer* reservedGlobalCBuffer;

	}pvt;
}

uint32_t Core::Graphics::getFrameBufferCount()
{
	return pvt.frameBufferCount;
}

uint32_t Core::Graphics::getFrameIndex()
{
	return pvt.frameIndex;
}

float Core::Graphics::getDeltaTime()
{
	return pvt.deltaTime;
}

float Core::Graphics::getTimeElapsed()
{
	return pvt.timeElapsed;
}

uint32_t Core::Graphics::getWidth()
{
	return pvt.width;
}

uint32_t Core::Graphics::getHeight()
{
	return pvt.height;
}

float Core::Graphics::getAspectRatio()
{
	return pvt.aspectRatio;
}

uint64_t Core::Graphics::getRenderedFrameCount()
{
	return pvt.renderedFrameCount;
}

D3D12_CPU_DESCRIPTOR_HANDLE Core::Graphics::getBackBufferHandle()
{
	return pvt.backBufferHandle;
}

Core::Resource::ImmutableCBuffer* Core::Graphics::getReservedGlobalCBuffer()
{
	return pvt.reservedGlobalCBuffer;
}

void Core::Graphics::Internal::initialize(const uint32_t frameBufferCount, const uint32_t width, const uint32_t height)
{
	pvt.frameBufferCount = frameBufferCount;

	pvt.width = width;

	pvt.height = height;

	pvt.aspectRatio = static_cast<float>(width) / static_cast<float>(height);
}

void Core::Graphics::Internal::renderedFrameCountInc()
{
	pvt.renderedFrameCount++;
}

void Core::Graphics::Internal::setFrameIndex(const uint32_t frameIndex)
{
	pvt.frameIndex = frameIndex;
}

void Core::Graphics::Internal::setDeltaTime(const float deltaTime)
{
	pvt.deltaTime = deltaTime;
}

void Core::Graphics::Internal::updateTimeElapsed()
{
	pvt.timeElapsed += pvt.deltaTime;
}

void Core::Graphics::Internal::setBackBufferHandle(const D3D12_CPU_DESCRIPTOR_HANDLE backBufferHandle)
{
	pvt.backBufferHandle = backBufferHandle;
}

void Core::Graphics::Internal::setReservedGlobalCBuffer(Resource::ImmutableCBuffer* const reservedGlobalCBuffer)
{
	pvt.reservedGlobalCBuffer = reservedGlobalCBuffer;
}
