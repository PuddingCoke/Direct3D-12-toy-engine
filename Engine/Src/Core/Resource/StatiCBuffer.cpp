#include<Gear/Core/Resource/StaticCBuffer.h>

#include<Gear/Core/Graphics.h>

StaticCBuffer::StaticCBuffer(Buffer* const buffer, const uint32_t size, const bool persistent) :
	ImmutableCBuffer(buffer, size, persistent)
{
	uploadHeaps = new UploadHeap * [Graphics::getFrameBufferCount()];

	for (uint32_t i = 0; i < Graphics::getFrameBufferCount(); i++)
	{
		uploadHeaps[i] = new UploadHeap(size);
	}
}

StaticCBuffer::~StaticCBuffer()
{
	if (uploadHeaps)
	{
		for (uint32_t i = 0; i < Graphics::getFrameBufferCount(); i++)
		{
			if (uploadHeaps[i])
			{
				delete uploadHeaps[i];
			}
		}
		delete[] uploadHeaps;
	}
}

StaticCBuffer::UpdateStruct StaticCBuffer::getUpdateStruct(const void* const data, const uint64_t size)
{
	uploadHeaps[Graphics::getFrameIndex()]->update(data, size);

	const UpdateStruct updateStruct = { buffer,uploadHeaps[Graphics::getFrameIndex()] };

	return updateStruct;
}
