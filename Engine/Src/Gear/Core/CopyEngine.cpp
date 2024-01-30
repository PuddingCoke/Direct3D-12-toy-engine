#include<Gear/Core/CopyEngine.h>

void CopyEngine::pushResourceCopy(Resource* const dstRes, Resource* const srcRes)
{
	std::lock_guard<std::mutex> guard(instance->updateMutex);

	instance->commandList->get()->CopyResource(dstRes->getResource(), srcRes->getResource());
}

void CopyEngine::pushBufferUpdate(Buffer* const buffer, const void* const data, const UINT dataSize)
{
	buffer->update(data, dataSize);

	std::lock_guard<std::mutex> guard(instance->updateMutex);

	instance->commandList->get()->CopyResource(buffer->getResource(), buffer->uploadHeaps.get()[buffer->uploadHeapIndex].getResource());
}

CopyEngine::CopyEngine() :
	commandList(new CommandList(D3D12_COMMAND_LIST_TYPE_COPY))
{
}

CopyEngine::~CopyEngine()
{
	delete commandList;
}
