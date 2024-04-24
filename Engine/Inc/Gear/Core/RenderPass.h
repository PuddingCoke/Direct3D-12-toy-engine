#pragma once

#ifndef _RENDERPASS_H_
#define _RENDERPASS_H_

#include<Gear/Core/GraphicsContext.h>
#include<Gear/Core/ResourceManager.h>

#include<Gear/Input/Mouse.h>
#include<Gear/Input/Keyboard.h>
#include<Gear/Utils/Math.h>
#include<Gear/Utils/Random.h>
#include<Gear/Utils/Utils.h>
#include<Gear/Utils/Timer.h>

#include<future>
#include<vector>
#include<unordered_set>

//provide needed resources for main render thread to solve pending state problems
struct RenderPassResult
{
	CommandList* transitionCMD;//helps transition pending state
	CommandList* renderCMD;//contains all pending state
};

class RenderPass
{
public:

	void beginRenderPass();

	RenderPassResult getRenderPassResult();

	RenderPass();

	virtual ~RenderPass();

	virtual void imGuiCommand();

protected:

	void begin();

	void end() const;

	virtual void recordCommand() = 0;

	GraphicsContext* const context;

	ResourceManager* const resManager;

private:

	CommandList* const transitionCMD;

	std::future<RenderPassResult> task;

};

#endif // !_RENDERPASS_H_
