#pragma once

#ifndef _RENDERTASK_H_
#define _RENDERTASK_H_

#include<Gear/Core/RenderEngine.h>

#include<Gear/Input/Mouse.h>
#include<Gear/Input/Keyboard.h>

#include<Gear/Utils/Math.h>
#include<Gear/Utils/Random.h>
#include<Gear/Utils/Utils.h>
#include<Gear/Utils/Timer.h>

#include<future>
#include<thread>
#include<condition_variable>
#include<mutex>

class RenderTask
{
public:

	RenderTask();

	virtual ~RenderTask();

	void beginTask();

	void waitTask();

	CommandList* getCommandList();

	virtual void imGUICall();

protected:

	//draw texture to backbuffer
	void blit(TextureRenderView* const texture) const;

	virtual void recordCommand() = 0;

	GraphicsContext* const context;

	ResourceManager* const resManager;

private:

	void workerLoop();

	std::thread workerThread;

	bool taskCompleted;

	bool isRunning;

	std::mutex taskMutex;

	std::condition_variable taskCondition;

};

template <typename RenderTaskType, typename First, typename... Rest>
std::future<void> createRenderTaskAsync(const First& first, const Rest&... args)
{
	return std::async(std::launch::async, [=]()
		{
			*first = new RenderTaskType(args...);

			RenderEngine::get()->submitCommandList((*first)->getCommandList());
		});
}

#endif // !_RENDERTASK_H_