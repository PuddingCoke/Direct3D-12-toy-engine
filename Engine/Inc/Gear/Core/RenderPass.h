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

private:

	friend class RenderEngine;

	void begin();

	virtual void recordCommand() = 0;

	void updateRefResStates();

	std::unordered_set<Resource*> referredResources;

	std::unordered_set<Buffer*> transitionBuffers;

	std::unordered_set<Texture*> transitionTextures;

	std::vector<Resource*> transientResources[Graphics::FrameBufferCount];

	std::vector<D3D12_RESOURCE_BARRIER> transitionBarriers;

	std::vector<PendingBufferBarrier> pendingBufferBarrier;

	std::vector<PendingTextureBarrier> pendingTextureBarrier;

	CommandList* transitionCMD;

	CommandList* renderCMD;

};

#endif // !_RENDERPASS_H_
