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

void Game::beginRenderPass(RenderPass* const renderPass)
{
	renderPass->beginRenderPass();

	recordQueue.push(renderPass);

	if (RenderEngine::get()->getDisplayImGuiSurface())
	{
		renderPass->imGUICall();
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
		RenderEngine::get()->submitCommandList(recordQueue.front()->getRenderPassResult());

		recordQueue.pop();
	}

	while (createQueue.size())
	{
		createQueue.front().get();

		createQueue.pop();
	}
}
