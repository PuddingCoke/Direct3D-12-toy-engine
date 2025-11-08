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

void Gear::Game::pushCreateAsync(Core::RenderThread* const renderThread)
{
	createQueue.push(renderThread);
}

void Gear::Game::scheduleAllTasks()
{
	while (recordQueue.size())
	{
		if (recordQueue.front()->waitTask())
		{
			throw "error occur while command recording";
		}

		Core::RenderEngine::submitCommandList(recordQueue.front()->getCommandList());

		recordQueue.pop();
	}

	while (createQueue.size())
	{
		if (createQueue.front()->waitInitialized())
		{
			throw "error occur while resource creation";
		}

		createQueue.pop();
	}
}
