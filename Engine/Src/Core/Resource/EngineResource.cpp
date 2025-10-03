#include<Gear/Core/Resource/EngineResource.h>

EngineResource::EngineResource(const bool persistent) :
	persistent(persistent), numSRVUAVCBVDescriptors(0), srvUAVCBVHandleStart()
{
}

EngineResource::~EngineResource()
{
}

void EngineResource::copyDescriptors()
{
}

bool EngineResource::getPersistent() const
{
	return persistent;
}

DescriptorHandle EngineResource::getTransientDescriptorHandle() const
{
	if (persistent)
	{
		LOGERROR(L"call getTransientDescriptorHandle for persistent resources is not allowed");
	}

	DescriptorHandle shaderVisibleHandle = GlobalDescriptorHeap::getResourceHeap()->allocDynamicDescriptor(numSRVUAVCBVDescriptors);

	GraphicsDevice::get()->CopyDescriptorsSimple(numSRVUAVCBVDescriptors, shaderVisibleHandle.getCPUHandle(), srvUAVCBVHandleStart, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	return shaderVisibleHandle;
}
