#include<Gear/Core/DX/Resource.h>

ID3D12Resource* Resource::getResource() const
{
	return resource.Get();
}

Resource::~Resource()
{
}

Resource::Resource(const ComPtr<ID3D12Resource>& resource, const bool stateTracking) :
	resource(resource), stateTracking(std::make_shared<bool>(stateTracking)), sharedResource(std::make_shared<bool>(false))
{
}

Resource::Resource(const D3D12_HEAP_PROPERTIES properties, const D3D12_HEAP_FLAGS flags, const D3D12_RESOURCE_DESC desc,
	const bool stateTracking, const D3D12_RESOURCE_STATES initialState, const D3D12_CLEAR_VALUE* clearValues) :
	stateTracking(std::make_shared<bool>(stateTracking)), sharedResource(std::make_shared<bool>(false))
{
	GraphicsDevice::get()->CreateCommittedResource(&properties, flags, &desc, initialState, clearValues, IID_PPV_ARGS(&resource));
}

Resource::Resource(Resource& res) :
	resource(res.resource),
	stateTracking(res.stateTracking),
	sharedResource(res.sharedResource)
{
	*(res.sharedResource) = true;
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

bool Resource::isSharedResource() const
{
	return *sharedResource;
}

void Resource::setName(LPCWSTR const name)
{
	resource->SetName(name);
}
