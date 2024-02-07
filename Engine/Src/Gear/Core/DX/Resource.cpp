#include<Gear/Core/DX/Resource/Resource.h>

ID3D12Resource* Resource::getResource() const
{
	return resource.Get();
}

Resource::~Resource()
{
}

Resource::Resource(const D3D12_HEAP_PROPERTIES* properties, D3D12_HEAP_FLAGS flags, const D3D12_RESOURCE_DESC* desc,
	const bool stateTracking, D3D12_RESOURCE_STATES initialState, const D3D12_CLEAR_VALUE* clearValues) :
	stateTracking(stateTracking), sharedResource(false)
{
	GraphicsDevice::get()->CreateCommittedResource(properties, flags, desc, initialState, clearValues, IID_PPV_ARGS(&resource));
}

Resource::Resource(Resource& res) :
	resource(res.resource),
	stateTracking(res.stateTracking),
	sharedResource(true)
{
	res.sharedResource = true;
}

void Resource::setStateTracking(const bool state)
{
	stateTracking = state;
}

bool Resource::isSharedResource() const
{
	return sharedResource;
}
