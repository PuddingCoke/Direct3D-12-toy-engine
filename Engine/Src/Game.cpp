#include<Gear/Game.h>

Game::Game()
{
}

Game::~Game()
{
}

void Game::imGUICall()
{
}

void Game::beginRenderTask(RenderTask* const renderTask)
{
	renderTask->beginTask();

	recordQueue.push(renderTask);

	if (Core::RenderEngine::getDisplayImGuiSurface())
	{
		renderTask->imGUICall();
	}
}

void Game::pushCreateFuture(std::future<void>&& future)
{
	createQueue.push(std::move(future));
}

void Game::scheduleAllTasks()
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
