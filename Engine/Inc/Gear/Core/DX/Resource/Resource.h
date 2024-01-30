#pragma once

#ifndef _RESOURCE_H_
#define _RESOURCE_H_

#include<Gear/Core/GraphicsDevice.h>

#include<memory>

enum class ResourceUsage
{
	STATIC,
	DYNAMIC
};

constexpr UINT D3D12_RESOURCE_STATE_UNKNOWN = 0xFFFFFFFF;

class Resource
{
public:

	ID3D12Resource* getResource();

	Resource() = delete;

	virtual ~Resource();

	Resource(const D3D12_HEAP_PROPERTIES* properties, D3D12_HEAP_FLAGS flags, const D3D12_RESOURCE_DESC* desc,
		const ResourceUsage usage, D3D12_RESOURCE_STATES initialState, const D3D12_CLEAR_VALUE* clearValues);

	virtual void updateGlobalStates() = 0;

	virtual void resetInternalStates() = 0;

private:

	ComPtr<ID3D12Resource> resource;

	ResourceUsage usage;

};

#endif // !_RESOURCE_H_