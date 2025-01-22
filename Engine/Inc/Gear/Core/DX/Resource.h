#pragma once

#ifndef _RESOURCE_H_
#define _RESOURCE_H_

#include<Gear/Core/GraphicsDevice.h>

#include<memory>

constexpr uint32_t D3D12_RESOURCE_STATE_UNKNOWN = 0xFFFFFFFF;

constexpr uint32_t D3D12_TRANSITION_ALL_MIPLEVELS = 0xFFFFFFFF;

//does a have b?
constexpr bool bitFlagSubset(const uint32_t a, const uint32_t b)
{
	return b && ((a & b) == b);
}

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

	uint32_t resourceIndex;

	struct TextureTransitionDesc
	{
		Texture* texture;
		uint32_t mipSlice;
	};

	struct BufferTransitionDesc
	{
		Buffer* buffer;
		Buffer* counterBuffer;
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
	uint32_t mipSlice;
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle;
};

struct DepthStencilDesc
{
	Texture* texture;
	uint32_t mipSlice;
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

struct ClearUAVDesc
{
	enum ResourceType
	{
		BUFFER,
		TEXTURE
	} type;

	struct TextureClearDesc
	{
		Texture* texture;
		uint32_t mipSlice;
	};

	struct BufferClearDesc
	{
		Buffer* buffer;
	};

	union
	{
		TextureClearDesc textureDesc;
		BufferClearDesc bufferDesc;
	};

	D3D12_GPU_DESCRIPTOR_HANDLE viewGPUHandle;

	D3D12_CPU_DESCRIPTOR_HANDLE viewCPUHandle;
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

	void setName(LPCWSTR const name);

private:

	ComPtr<ID3D12Resource> resource;

	bool stateTracking;

	std::shared_ptr<bool> sharedResource;

};

#endif // !_RESOURCE_H_