#include<Gear/Core/Resource/D3D12Resource/Resource.h>

ID3D12Resource* Resource::getResource() const
{
	return resource.Get();
}

Resource::~Resource()
{
}

Resource::Resource(const ComPtr<ID3D12Resource>& resource, const bool stateTracking) :
	resource(resource), stateTracking(std::make_shared<bool>(stateTracking)), sharedResource(std::make_shared<bool>(false)),
	inReferredList(false), inTrackingList(false)
{
}

Resource::Resource(const D3D12_HEAP_PROPERTIES properties, const D3D12_HEAP_FLAGS flags, const D3D12_RESOURCE_DESC desc,
	const bool stateTracking, const D3D12_RESOURCE_STATES initialState, const D3D12_CLEAR_VALUE* clearValues) :
	stateTracking(std::make_shared<bool>(stateTracking)), sharedResource(std::make_shared<bool>(false)),
	inReferredList(false), inTrackingList(false)
{
	GraphicsDevice::get()->CreateCommittedResource(&properties, flags, &desc, initialState, clearValues, IID_PPV_ARGS(&resource));
}

Resource::Resource(Resource* const res) :
	resource(res->resource), stateTracking(res->stateTracking), sharedResource(res->sharedResource),
	inReferredList(false), inTrackingList(false)
{
	*sharedResource = true;
}

D3D12_GPU_VIRTUAL_ADDRESS Resource::getGPUAddress() const
{
	return resource->GetGPUVirtualAddress();
}

void Resource::setStateTracking(const bool state)
{
	*stateTracking = state;
}

bool Resource::getStateTracking() const
{
	return *stateTracking;
}

bool Resource::getSharedResource() const
{
	return *sharedResource;
}

void Resource::setName(LPCWSTR const name) const
{
	resource->SetName(name);
}

bool Resource::getInTrackingList() const
{
	return inTrackingList;
}

void Resource::pushToReferredList(std::vector<Resource*>& referredList)
{
	//only update global state for shared resource that need state tracking
	if (getStateTracking() && getSharedResource() && !inReferredList)
	{
		referredList.push_back(this);

		inReferredList = true;
	}
}

void Resource::popFromReferredList()
{
	inReferredList = false;
}

void Resource::pushToTrackingList()
{
	inTrackingList = true;
}

void Resource::popFromTrackingList()
{
	inTrackingList = false;
}
