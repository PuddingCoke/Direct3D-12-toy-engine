#pragma once

#ifndef _RESOURCE_H_
#define _RESOURCE_H_

#include<Gear/Core/GraphicsDevice.h>
#include<Gear/Core/GlobalDescriptorHeap.h>

#include<memory>

//texture transition requirement
//getRTV texture mipSlice RTV rtvHandle
//getDSV texture mipSlice DSV dsvHandle
//getSRV texture mipSlice SRV resourceIndex
//getUAV texture mipSlice UAV resourceIndex


//buffer transition requirement
//getSRV buffer state resourceIndex
//getUAV buffer state resourceIndex

constexpr UINT D3D12_RESOURCE_STATE_UNKNOWN = 0xFFFFFFFF;

class Buffer;
class Texture;

struct ShaderResourceDesc
{
	enum ResourceType
	{
		BUFFER,
		TEXTURE
	} type;

	enum TargetStates
	{
		SRV,
		UAV,
		CBV
	} state;

	struct TextureTransitionDesc
	{
		Texture* texture;
		UINT mipSlice;
		UINT resourceIndex;
	};

	struct BufferTransitionDesc
	{
		Buffer* buffer;
		UINT resourceIndex;
	};

	union
	{
		TextureTransitionDesc textureDesc;
		BufferTransitionDesc bufferDesc;
	};
};

struct RenderTargetDesc
{
	Texture* texture;
	UINT mipSlice;
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle;
};

struct DepthStencilDesc
{
	Texture* texture;
	UINT mipSlice;
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle;
};

class Resource
{
public:

	ID3D12Resource* getResource() const;

	virtual ~Resource();

	Resource(const bool stateTracking);

	Resource(const D3D12_HEAP_PROPERTIES properties, const D3D12_HEAP_FLAGS flags, const D3D12_RESOURCE_DESC desc,
		const bool stateTracking, const D3D12_RESOURCE_STATES initialState, const D3D12_CLEAR_VALUE* clearValues);

	Resource(Resource&);

	Resource() = delete;

	Resource(const Resource&) = delete;

	void operator=(const Resource&) = delete;

	virtual void updateGlobalStates() = 0;

	virtual void resetInternalStates() = 0;

	virtual void resetTransitionStates() = 0;

	D3D12_GPU_VIRTUAL_ADDRESS getGPUAddress() const;

	void setStateTracking(const bool state);

	bool getStateTracking() const;

	bool isSharedResource() const;

protected:

	void createResource(const D3D12_HEAP_PROPERTIES properties, const D3D12_HEAP_FLAGS flags, const D3D12_RESOURCE_DESC desc,
		const D3D12_RESOURCE_STATES initialState, const D3D12_CLEAR_VALUE* clearValues);

	ID3D12Resource** releaseAndGet();

private:

	ComPtr<ID3D12Resource> resource;

	bool stateTracking;

	bool sharedResource;
};

#endif // !_RESOURCE_H_