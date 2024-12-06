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

void Game::submitRenderPass(RenderPass* const pass)
{
	if (RenderEngine::get()->getDisplayImGuiSurface())
	{
		pass->imGUICall();
	}

	RenderEngine::get()->submitRecordCommandList(pass->getRenderPassResult());
}
