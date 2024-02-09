#pragma once

#ifndef _RENDERPASS_H_
#define _RENDERPASS_H_

#include<Gear/Core/CommandList.h>
#include<Gear/Core/DX/Resource/Resource.h>
#include<Gear/Core/DX/Resource/Buffer.h>
#include<Gear/Core/DX/Resource/Texture.h>

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
	void setGraphicsGlobalIndexBuffer();

	//per frame
	void setComputeGlobalIndexBuffer();

	void setGraphicsConstants();

	void setComputeConstants();

	void setVertexIndexBuffer();

	void setHullIndexBuffer();

	void setGeometryIndexBuffer();

	void setDomainIndexBuffer();

	void setPixelIndexBuffer();

	void setComputeIndexBuffer();

	void setMeshIndexBuffer();

	void setAmplificationIndexBuffer();

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
