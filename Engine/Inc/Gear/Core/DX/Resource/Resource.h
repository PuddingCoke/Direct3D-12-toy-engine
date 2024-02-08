#pragma once

#ifndef _RESOURCE_H_
#define _RESOURCE_H_

#include<Gear/Core/GraphicsDevice.h>

#include<memory>

constexpr UINT D3D12_RESOURCE_STATE_UNKNOWN = 0xFFFFFFFF;

class Resource
{
public:

	ID3D12Resource* getResource() const;

	virtual ~Resource();

	Resource(const D3D12_HEAP_PROPERTIES properties, const D3D12_HEAP_FLAGS flags, const D3D12_RESOURCE_DESC desc,
		const bool stateTracking, const D3D12_RESOURCE_STATES initialState, const D3D12_CLEAR_VALUE* clearValues);

	Resource(Resource&);

	Resource(const Resource&) = delete;

	void operator=(const Resource&) = delete;

	virtual void updateGlobalStates() = 0;

	virtual void resetInternalStates() = 0;

	virtual void resetTransitionStates() = 0;

	void setStateTracking(const bool state);

	bool isSharedResource() const;

private:

	ComPtr<ID3D12Resource> resource;

	bool stateTracking;

	bool sharedResource;

};

#endif // !_RESOURCE_H_