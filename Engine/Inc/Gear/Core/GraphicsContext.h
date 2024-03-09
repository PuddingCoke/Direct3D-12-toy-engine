#pragma once

#ifndef _GRAPHICSCONTEXT_H_
#define _GRAPHICSCONTEXT_H_

#include<Gear/Core/DX/Resource/Buffer.h>
#include<Gear/Core/DX/Resource/Texture.h>

#include<Gear/Core/CommandList.h>
#include<Gear/Core/Resource/ConstantBuffer.h>
#include<Gear/Core/Resource/VertexBuffer.h>
#include<Gear/Core/Resource/IndexBuffer.h>
#include<Gear/Core/Resource/IndexConstantBuffer.h>

#include<vector>
#include<unordered_set>


class GraphicsContext
{
public:
	
	GraphicsContext();

	~GraphicsContext();

	void updateBuffer(VertexBuffer* const vb, const void* const data, const UINT size);

	void updateBuffer(IndexBuffer* const ib, const void* const data, const UINT size);

	//per frame global resources transition immediate
	void setGlobalIndexBuffer(const IndexConstantBuffer* const indexBuffer);

	//per draw call transition immediate
	void setComputeIndexBuffer(const IndexConstantBuffer* const indexBuffer);

	//beware use of these two method provide resourceIndex as long as resource's stateTracking is disabled

	void setGraphicsConstants(const std::initializer_list<ShaderResourceDesc> descs, const UINT offset);

	void setGraphicsConstants(const UINT numValues, const void* const data, const UINT offset) const;

	void setComputeConstants(const std::initializer_list<ShaderResourceDesc> descs, const UINT offset);

	void setComputeConstants(const UINT numValues, const void* const data, const UINT offset) const;

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

	void setRenderTargets(const std::initializer_list<RenderTargetDesc>& renderTargets, const std::initializer_list<DepthStencilDesc>& depthStencils);

	void setDefRenderTarget() const;

	void clearDefRenderTarget(const FLOAT clearValue[4]) const;

	void setVertexBuffers(const UINT startSlot, const std::initializer_list<VertexBuffer*>& vertexBuffers);

	void setIndexBuffers(const std::initializer_list<IndexBuffer*>& indexBuffers);

	void setTopology(const D3D12_PRIMITIVE_TOPOLOGY topology) const;

	void setViewport(const float width, const float height);

	void setViewport(const UINT width, const UINT height);

	void setScissorRect(const UINT left, const UINT top, const UINT right, const UINT bottom);

	void setScissorRect(const float left, const float top, const float right, const float bottom);

	void setPipelineState(ID3D12PipelineState* const pipelineState) const;

	void clearRenderTarget(const RenderTargetDesc desc, const FLOAT clearValue[4]);

	void clearDepthStencil(const DepthStencilDesc desc, const D3D12_CLEAR_FLAGS flags, const FLOAT depth, const UINT8 stencil);

	void uavBarrier(const std::initializer_list<Resource*>& resources);

	void draw(const UINT vertexCountPerInstance, const UINT instanceCount, const UINT startVertexLocation, const UINT startInstanceLocation);

	void begin();

	void end();

	CommandList* getCommandList() const;

private:

	friend class RenderEngine;

	static ConstantBuffer* globalConstantBuffer;

	void updateReferredResStates();

	void flushTransitionResources();

	void pushGraphicsStageIndexBuffer(UINT rootParameterIndex, const IndexConstantBuffer* const indexBuffer, const UINT targetSRVState);

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

	CommandList* commandList;

};


#endif // !_GRAPHICSCONTEXT_H_
