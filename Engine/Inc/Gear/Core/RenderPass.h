#pragma once

#ifndef _RENDERPASS_H_
#define _RENDERPASS_H_

#include<Gear/Core/CommandList.h>
#include<Gear/Core/Resource/IndexConstantBuffer.h>
#include<Gear/Core/Resource/TextureRenderTarget.h>
#include<Gear/Core/Resource/TextureDepthStencil.h>
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

	//per frame transition immediate
	void setGraphicsGlobalIndexBuffer(const IndexConstantBuffer* const globalIndexBuffer);

	//per frame transition immediate
	void setComputeGlobalIndexBuffer(const IndexConstantBuffer* const globalIndexBuffer);

	//per draw call transition immediate
	void setComputeIndexBuffer(const IndexConstantBuffer* const indexBuffer);

	void setGraphicsConstants();

	void setComputeConstants();

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

	void setRenderTargets(const std::initializer_list<TransitionDesc>& renderTargets,const std::initializer_list<TransitionDesc>& depthStencils);

	void setVertexBuffers(const UINT startSlot,const std::initializer_list<VertexBuffer*>& vertexBuffers);

	void setIndexBuffers(const std::initializer_list<IndexBuffer*>& indexBuffers);

	void setTopology(const D3D12_PRIMITIVE_TOPOLOGY topology);

	//when create a buffer cpuWritable should be true
	void updateBuffer(Buffer* const buffer, const void* const dataPtr, const UINT dataSize);

	void begin();

	void end();

	virtual void recordCommand() = 0;

private:

	friend class RenderEngine;

	void updateReferredResStates();

	void flushTransitionResources();

	void pushGraphicsStageIndexBuffer(UINT rootParameterIndex, const IndexConstantBuffer* const indexBuffer,const UINT targetSRVState);

	void pushResourceTrackList(Texture* const texture);

	void pushResourceTrackList(Buffer* const buffer);

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
