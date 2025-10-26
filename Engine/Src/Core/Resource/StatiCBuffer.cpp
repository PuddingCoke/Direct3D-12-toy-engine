#include<Gear/Core/Resource/StaticCBuffer.h>

#include<Gear/Core/Graphics.h>

StaticCBuffer::StaticCBuffer(Buffer* const buffer, const uint32_t size, const bool persistent) :
	ImmutableCBuffer(buffer, size, persistent)
{
	uploadHeaps = new UploadHeap * [Core::Graphics::getFrameBufferCount()];

	dataPtrs = new void* [Core::Graphics::getFrameBufferCount()];

	for (uint32_t i = 0; i < Core::Graphics::getFrameBufferCount(); i++)
	{
		uploadHeaps[i] = new UploadHeap(size);

		dataPtrs[i] = uploadHeaps[i]->map();
	}
}

StaticCBuffer::~StaticCBuffer()
{
	if (uploadHeaps)
	{
		for (uint32_t i = 0; i < Core::Graphics::getFrameBufferCount(); i++)
		{
			if (uploadHeaps[i])
			{
				uploadHeaps[i]->unmap();

				delete uploadHeaps[i];
			}
		}

		delete[] uploadHeaps;
	}

	if (dataPtrs)
	{
		delete[] dataPtrs;
	}
}

StaticCBuffer::UpdateStruct StaticCBuffer::getUpdateStruct(const void* const data, const uint64_t size)
{
	memcpy(dataPtrs[Core::Graphics::getFrameIndex()], data, size);

	const UpdateStruct updateStruct = { buffer,uploadHeaps[Core::Graphics::getFrameIndex()] };

	return updateStruct;
}
