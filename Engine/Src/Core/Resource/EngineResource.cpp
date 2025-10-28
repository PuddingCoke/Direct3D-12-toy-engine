#include<Gear/Core/Resource/EngineResource.h>

Core::Resource::EngineResource::EngineResource(const bool persistent) :
	persistent(persistent), numSRVUAVCBVDescriptors(0), srvUAVCBVHandleStart()
{
}

Core::Resource::EngineResource::~EngineResource()
{
}

void Core::Resource::EngineResource::copyDescriptors()
{
}

bool Core::Resource::EngineResource::getPersistent() const
{
	return persistent;
}

Core::D3D12Core::DescriptorHandle Core::Resource::EngineResource::getTransientDescriptorHandle() const
{
#ifdef _DEBUG
	if (persistent)
	{
		LOGERROR(L"call getTransientDescriptorHandle for persistent resources is not allowed");
	}
#endif // _DEBUG
	
	D3D12Core::DescriptorHandle shaderVisibleHandle = Core::GlobalDescriptorHeap::getResourceHeap()->allocDynamicDescriptor(numSRVUAVCBVDescriptors);

	Core::GraphicsDevice::get()->CopyDescriptorsSimple(numSRVUAVCBVDescriptors, shaderVisibleHandle.getCPUHandle(), srvUAVCBVHandleStart, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	return shaderVisibleHandle;
}
