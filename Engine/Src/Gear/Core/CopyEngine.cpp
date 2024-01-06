#include<Gear/Core/CopyEngine.h>

void CopyEngine::pushResourceCopy(Resource* const dstRes, Resource* const srcRes)
{
	std::lock_guard<std::mutex> guard(instance->updateMutex);

	instance->commandList->get()->CopyResource(dstRes->getResource(), srcRes->getResource());
}

CopyEngine::CopyEngine() :
	commandList(new CommandList(D3D12_COMMAND_LIST_TYPE_COPY))
{
}

CopyEngine::~CopyEngine()
{
	delete commandList;
}
