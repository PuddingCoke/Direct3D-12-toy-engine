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

	UINT resourceIndex;

	struct TextureTransitionDesc
	{
		Texture* texture;
		UINT mipSlice;
	};

	struct BufferTransitionDesc
	{
		Buffer* buffer;
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

struct VertexBufferDesc
{
	Buffer* buffer;
	D3D12_VERTEX_BUFFER_VIEW vbv;
};

struct IndexBufferDesc
{
	Buffer* buffer;
	D3D12_INDEX_BUFFER_VIEW ibv;
};

class Resource
{
public:

	ID3D12Resource* getResource() const;

	virtual ~Resource();

	Resource(const ComPtr<ID3D12Resource>& resource, const bool stateTracking);

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

private:

	ComPtr<ID3D12Resource> resource;

	bool stateTracking;

	bool sharedResource;
};

#endif // !_RESOURCE_H_