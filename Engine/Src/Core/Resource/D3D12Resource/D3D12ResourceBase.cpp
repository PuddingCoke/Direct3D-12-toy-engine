#include<Gear/Core/Resource/D3D12Resource/D3D12ResourceBase.h>

ID3D12Resource* Core::Resource::D3D12Resource::D3D12ResourceBase::getResource() const
{
	return resource.Get();
}

Core::Resource::D3D12Resource::D3D12ResourceBase::~D3D12ResourceBase()
{
}

Core::Resource::D3D12Resource::D3D12ResourceBase::D3D12ResourceBase(const ComPtr<ID3D12Resource>& resource, const bool stateTracking) :
	resource(resource), stateTracking(std::make_shared<bool>(stateTracking)), sharedResource(std::make_shared<bool>(false)),
	inReferredList(false), inTrackingList(false)
{
}

Core::Resource::D3D12Resource::D3D12ResourceBase::D3D12ResourceBase(const D3D12_HEAP_PROPERTIES properties, const D3D12_HEAP_FLAGS flags, const D3D12_RESOURCE_DESC desc,
	const bool stateTracking, const D3D12_RESOURCE_STATES initialState, const D3D12_CLEAR_VALUE* clearValues) :
	stateTracking(std::make_shared<bool>(stateTracking)), sharedResource(std::make_shared<bool>(false)),
	inReferredList(false), inTrackingList(false)
{
	Core::GraphicsDevice::get()->CreateCommittedResource(&properties, flags, &desc, initialState, clearValues, IID_PPV_ARGS(&resource));
}

Core::Resource::D3D12Resource::D3D12ResourceBase::D3D12ResourceBase(D3D12ResourceBase* const res) :
	resource(res->resource), stateTracking(res->stateTracking), sharedResource(res->sharedResource),
	inReferredList(false), inTrackingList(false)
{
	*sharedResource = true;
}

D3D12_GPU_VIRTUAL_ADDRESS Core::Resource::D3D12Resource::D3D12ResourceBase::getGPUAddress() const
{
	return resource->GetGPUVirtualAddress();
}

void Core::Resource::D3D12Resource::D3D12ResourceBase::setStateTracking(const bool state)
{
	*stateTracking = state;
}

bool Core::Resource::D3D12Resource::D3D12ResourceBase::getStateTracking() const
{
	return *stateTracking;
}

bool Core::Resource::D3D12Resource::D3D12ResourceBase::getSharedResource() const
{
	return *sharedResource;
}

void Core::Resource::D3D12Resource::D3D12ResourceBase::setName(LPCWSTR const name) const
{
	resource->SetName(name);
}

bool Core::Resource::D3D12Resource::D3D12ResourceBase::getInTrackingList() const
{
	return inTrackingList;
}

void Core::Resource::D3D12Resource::D3D12ResourceBase::pushToReferredList(std::vector<D3D12ResourceBase*>& referredList)
{
	//只为需要状态追踪的共享资源更新它的全局状态
	if (getStateTracking() && getSharedResource() && !inReferredList)
	{
		referredList.push_back(this);

		inReferredList = true;
	}
}

void Core::Resource::D3D12Resource::D3D12ResourceBase::popFromReferredList()
{
	inReferredList = false;
}

void Core::Resource::D3D12Resource::D3D12ResourceBase::pushToTrackingList()
{
	inTrackingList = true;
}

void Core::Resource::D3D12Resource::D3D12ResourceBase::popFromTrackingList()
{
	inTrackingList = false;
}
