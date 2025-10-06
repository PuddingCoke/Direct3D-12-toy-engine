#pragma once

#ifndef _RENDERTASK_H_
#define _RENDERTASK_H_

#include<Gear/Core/RenderEngine.h>

#include<Gear/Core/ResourceManager.h>

#include<Gear/Core/PipelineState.h>

#include<Gear/Core/Graphics.h>

#include<Gear/Input/Mouse.h>

#include<Gear/Input/Keyboard.h>

#include<Gear/Utils/Math.h>

#include<Gear/Utils/Random.h>

#include<Gear/Utils/Utils.h>

#include<Gear/Utils/Timer.h>

#include<Gear/Utils/Color.h>

#include<future>

class RenderTask
{
public:

	RenderTask(const RenderTask&) = delete;

	void operator=(const RenderTask&) = delete;

	RenderTask();

	virtual ~RenderTask();

	void beginTask();

	void waitTask();

	CommandList* getCommandList() const;

	virtual void imGUICall();

protected:

	//draw texture to backbuffer
	void blit(TextureRenderView* const texture) const;

	virtual void recordCommand() = 0;

	ResourceManager* const resManager;

	GraphicsContext* const context;

private:

	void workerLoop();

	bool taskCompleted;

	bool isRunning;

	std::mutex taskMutex;

	std::condition_variable taskCondition;

	std::thread workerThread;

};

template <typename First, typename... Rest>
std::future<void> createRenderTaskAsync(const First& first, const Rest&... args)
{
	using RenderTaskType = std::remove_pointer_t<std::remove_pointer_t<First>>;

	return std::async(std::launch::async, [=]()
		{
			*first = new RenderTaskType(args...);

			RenderEngine::get()->submitCommandList((*first)->getCommandList());
		});
}

#endif // !_RENDERTASK_H_