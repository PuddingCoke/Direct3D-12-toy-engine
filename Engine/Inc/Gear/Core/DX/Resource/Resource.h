#pragma once

#ifndef _RESOURCE_H_
#define _RESOURCE_H_

#include<Gear/Core/GraphicsDevice.h>

class Resource
{
public:

	ID3D12Resource* getResource();

	Resource() = delete;

	virtual ~Resource();

	Resource(const D3D12_HEAP_PROPERTIES* properties, D3D12_HEAP_FLAGS flags,
		const D3D12_RESOURCE_DESC* desc, D3D12_RESOURCE_STATES initialState, const D3D12_CLEAR_VALUE* clearValues);

private:

	ComPtr<ID3D12Resource> resource;

};

#endif // !_RESOURCE_H_
