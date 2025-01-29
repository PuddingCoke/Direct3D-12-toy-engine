#pragma once

#ifndef _ENGINERESOURCE_H_
#define _ENGINERESOURCE_H_

#include<Gear/Core/GlobalDescriptorHeap.h>

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

class EngineResource
{
public:

	EngineResource() = delete;

	EngineResource(const bool persistent);

	virtual ~EngineResource();

	virtual void copyDescriptors();

	bool getPersistent() const;

protected:

	//copy non shader visible resource heap to shader visible resource heap
	DescriptorHandle getTransientDescriptorHandle() const;

	const bool persistent;

	D3D12_CPU_DESCRIPTOR_HANDLE srvUAVCBVHandleStart;

	uint32_t numSRVUAVCBVDescriptors;

};

#endif // !_ENGINERESOURCE_H_