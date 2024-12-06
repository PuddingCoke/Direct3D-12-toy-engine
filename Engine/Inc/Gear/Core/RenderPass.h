#pragma once

#ifndef _RENDERPASS_H_
#define _RENDERPASS_H_

#include<Gear/Core/RenderEngine.h>

#include<Gear/Input/Mouse.h>
#include<Gear/Input/Keyboard.h>

#include<Gear/Utils/Math.h>
#include<Gear/Utils/Random.h>
#include<Gear/Utils/Utils.h>
#include<Gear/Utils/Timer.h>

#include<future>

class RenderPass
{
public:

	RenderPass();

	virtual ~RenderPass();

	void beginRenderPass();

	CommandList* getRenderPassResult();

	virtual void imGUICall();

protected:

	//draw texture to backbuffer
	void blit(TextureRenderView* const texture) const;

	virtual void recordCommand() = 0;

	GraphicsContext* const context;

	ResourceManager* const resManager;

private:

	std::future<CommandList*> task;

};

#endif // !_RENDERPASS_H_
