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

private:

	friend class RenderEngine;

	//bind fixed state for each commandlist
	//root signature & descriptor heap and etc.
	void setFixedStates();

	virtual void recordCommand() = 0;

	void updateRefResStates();

	std::unordered_set<Resource*> refResources;

	std::vector<PendingBufferBarrier> pendingBufferBarrier;

	std::vector<PendingTextureBarrier> pendingTextureBarrier;

	CommandList* transitionCMD;

	CommandList* renderCMD;

};

#endif // !_RENDERPASS_H_
