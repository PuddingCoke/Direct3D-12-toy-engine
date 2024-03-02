#pragma once

#ifndef _RENDERPASS_H_
#define _RENDERPASS_H_

#include<Gear/Core/CommandList.h>
#include<Gear/Core/Resource/IndexConstantBuffer.h>
#include<Gear/Core/Resource/TextureRenderTarget.h>
#include<Gear/Core/Resource/TextureDepthStencil.h>
#include<Gear/Core/Resource/ConstantBuffer.h>
#include<Gear/Core/Resource/VertexBuffer.h>
#include<Gear/Core/Resource/IndexBuffer.h>

#include<future>
#include<vector>
#include<unordered_set>

class RenderPass
{
public:

	std::future<void> getPassResult();

	RenderPass();

	virtual ~RenderPass();

protected:

	ConstantBuffer* CreateConstantBuffer(const UINT size, const bool cpuWritable, const void* const data);

	IndexBuffer* CreateIndexBuffer(const DXGI_FORMAT format, const UINT size, const bool cpuWritable, const void* const data);

	IndexConstantBuffer* CreateIndexConstantBuffer(const std::initializer_list<ShaderResourceDesc>& descs, const bool cpuWritable);

	IndexConstantBuffer* CreateIndexConstantBuffer(const UINT indicesNum, const bool cpuWritable);

	TextureDepthStencil* CreateTextureDepthStencil(const UINT width, const UINT height, const DXGI_FORMAT resFormat, const UINT arraySize, const UINT mipLevels, const bool isTextureCube);

	TextureRenderTarget* CreateTextureRenderTarget(const TextureViewCreationFlags flags, const UINT width, const UINT height, const DXGI_FORMAT resFormat, const UINT arraySize, const UINT mipLevels,
		const DXGI_FORMAT srvFormat, const DXGI_FORMAT uavFormat, const DXGI_FORMAT rtvFormat, const bool isTextureCube);

	TextureRenderTarget* CreateTextureRenderTarget(const TextureViewCreationFlags flags, const std::string filePath,
		const DXGI_FORMAT srvFormat, const DXGI_FORMAT uavFormat, const DXGI_FORMAT rtvFormat, const bool isTextureCube);

	VertexBuffer* CreateVertexBuffer(const UINT perVertexSize, const UINT size, const bool stateTracking, const bool cpuWritable, const void* const data);

	//per frame global resources transition immediate
	void setGlobalIndexBuffer(const IndexConstantBuffer* const indexBuffer);

	//per draw call transition immediate
	void setComputeIndexBuffer(const IndexConstantBuffer* const indexBuffer);

	//beware use of these two method provide resourceIndex as long as resource's stateTracking is disabled

	void setGraphicsConstants(const UINT numValues, const void* const data, const UINT offset);

	void setComputeConstants(const UINT numValues, const void* const data, const UINT offset);

	//deferred because a srv might be read in pixel shader or non pixel shader so the final state is pending
	//but for compute shader index buffer and global graphics&compute index buffer transition state is deterministic

	//per draw call transition deferred
	void setVertexIndexBuffer(const IndexConstantBuffer* const indexBuffer);

	//per draw call transition deferred
	void setHullIndexBuffer(const IndexConstantBuffer* const indexBuffer);

	//per draw call transition deferred
	void setGeometryIndexBuffer(const IndexConstantBuffer* const indexBuffer);

	//per draw call transition deferred
	void setDomainIndexBuffer(const IndexConstantBuffer* const indexBuffer);

	//per draw call transition deferred
	void setPixelIndexBuffer(const IndexConstantBuffer* const indexBuffer);

	//per draw call transition deferred
	void setMeshIndexBuffer(const IndexConstantBuffer* const indexBuffer);

	//per draw call transition deferred
	void setAmplificationIndexBuffer(const IndexConstantBuffer* const indexBuffer);

	void finishGraphicsStageIndexBuffer();

	void setRenderTargets(const std::initializer_list<RenderTargetDesc>& renderTargets,const std::initializer_list<DepthStencilDesc>& depthStencils);

	void setDefRenderTarget();

	void clearDefRenderTarget(const FLOAT clearValue[4]);

	void setVertexBuffers(const UINT startSlot,const std::initializer_list<VertexBuffer*>& vertexBuffers);

	void setIndexBuffers(const std::initializer_list<IndexBuffer*>& indexBuffers);

	void setTopology(const D3D12_PRIMITIVE_TOPOLOGY topology);

	void setViewport(const float width, const float height);

	void setViewport(const UINT width, const UINT height);

	void setScissorRect(const UINT left, const UINT top, const UINT right, const UINT bottom);

	void setScissorRect(const float left, const float top, const float right, const float bottom);

	void setPipelineState(ID3D12PipelineState* const pipelineState);

	void clearRenderTarget(const RenderTargetDesc desc, const FLOAT clearValue[4]);

	void clearDepthStencil(const DepthStencilDesc desc, const D3D12_CLEAR_FLAGS flags, const FLOAT depth, const UINT8 stencil);

	void begin();

	void end();

	virtual void recordCommand() = 0;

private:

	friend class RenderEngine;

	static ConstantBuffer* globalConstantBuffer;

	void updateReferredResStates();

	void flushTransitionResources();

	void pushGraphicsStageIndexBuffer(UINT rootParameterIndex, const IndexConstantBuffer* const indexBuffer,const UINT targetSRVState);

	void pushResourceTrackList(Texture* const texture);

	void pushResourceTrackList(Buffer* const buffer);

	D3D12_VIEWPORT vp;

	D3D12_RECT rt;

	std::unordered_set<Resource*> referredResources;

	std::unordered_set<Buffer*> transitionBuffers;

	std::unordered_set<Texture*> transitionTextures;

	std::vector<D3D12_RESOURCE_BARRIER> transitionBarriers;

	std::vector<PendingBufferBarrier> pendingBufferBarrier;

	std::vector<PendingTextureBarrier> pendingTextureBarrier;

	std::vector<Resource*> transientResources[Graphics::FrameBufferCount];

	CommandList* transitionCMD;

	CommandList* renderCMD;

};

#endif // !_RENDERPASS_H_
