#pragma once

#include<Gear/Game.h>

#include"MyRenderPass.h"

class MyGame :public Game
{
public:

	MyGame():
		renderPass(new MyRenderPass())
	{
	}

	~MyGame()
	{
		delete renderPass;
	}

	void update(const float dt) override
	{

	}

	void render()
	{
		auto result = renderPass->getPassResult();

		RenderEngine::get()->submitRenderPass(renderPass);
	}

	MyRenderPass* renderPass;
	

};