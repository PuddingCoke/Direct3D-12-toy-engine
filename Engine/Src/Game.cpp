#include<Gear/Game.h>

Gear::Game::Game()
{
}

Gear::Game::~Game()
{
}

void Gear::Game::imGUICall()
{
}

void Gear::Game::beginRenderTask(Core::RenderTask* const renderTask)
{
	renderTask->beginTask();

	recordQueue.push(renderTask);

	if (Core::RenderEngine::getDisplayImGuiSurface())
	{
		renderTask->imGUICall();
	}
}

void Gear::Game::pushCreateFuture(std::future<void>&& future)
{
	createQueue.push(std::move(future));
}

void Gear::Game::scheduleAllTasks()
{
	while (recordQueue.size())
	{
		recordQueue.front()->waitTask();

		Core::RenderEngine::submitCommandList(recordQueue.front()->getCommandList());

		recordQueue.pop();
	}

	while (createQueue.size())
	{
		createQueue.front().get();

		createQueue.pop();
	}
}
