#pragma once

#ifndef _RENDERPASS_H_
#define _RENDERPASS_H_

#include<Gear/Core/CommandList.h>
#include<Gear/Core/DX/Resource/Resource.h>
#include<Gear/Core/DX/Resource/Buffer.h>
#include<Gear/Core/DX/Resource/Texture.h>
#include<Gear/Core/Resource/IndexConstantBuffer.h>

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

	//per frame
	void setGraphicsGlobalIndexBuffer(const IndexConstantBuffer& globalIndexBuffer);

	//per frame
	void setComputeGlobalIndexBuffer(const IndexConstantBuffer& globalIndexBuffer);

	void setGraphicsConstants();

	void setComputeConstants();

	void setVertexIndexBuffer(const IndexConstantBuffer& indexBuffer);

	void setHullIndexBuffer(const IndexConstantBuffer& indexBuffer);

	void setGeometryIndexBuffer(const IndexConstantBuffer& indexBuffer);

	void setDomainIndexBuffer(const IndexConstantBuffer& indexBuffer);

	void setPixelIndexBuffer(const IndexConstantBuffer& indexBuffer);

	void setComputeIndexBuffer(const IndexConstantBuffer& indexBuffer);

	void setMeshIndexBuffer(const IndexConstantBuffer& indexBuffer);

	void setAmplificationIndexBuffer(const IndexConstantBuffer& indexBuffer);

	void begin();

	void end();

	virtual void recordCommand() = 0;

private:

	friend class RenderEngine;

	void updateReferredResStates();

	void flushTransitionResources();

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
