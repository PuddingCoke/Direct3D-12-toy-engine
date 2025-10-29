#include<Gear/Core/Resource/ResourceBase.h>

Gear::Core::Resource::ResourceBase::ResourceBase(const bool persistent) :
	persistent(persistent), numSRVUAVCBVDescriptors(0), srvUAVCBVHandleStart()
{
}

Gear::Core::Resource::ResourceBase::~ResourceBase()
{
}

void Gear::Core::Resource::ResourceBase::copyDescriptors()
{
}

bool Gear::Core::Resource::ResourceBase::getPersistent() const
{
	return persistent;
}

Gear::Core::D3D12Core::DescriptorHandle Gear::Core::Resource::ResourceBase::getTransientDescriptorHandle() const
{
#ifdef _DEBUG
	if (persistent)
	{
		LOGERROR(L"call getTransientDescriptorHandle for persistent resources is not allowed");
	}
#endif // _DEBUG
	
	D3D12Core::DescriptorHandle shaderVisibleHandle = GlobalDescriptorHeap::getResourceHeap()->allocDynamicDescriptor(numSRVUAVCBVDescriptors);

	GraphicsDevice::get()->CopyDescriptorsSimple(numSRVUAVCBVDescriptors, shaderVisibleHandle.getCPUHandle(), srvUAVCBVHandleStart, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	return shaderVisibleHandle;
}
