#pragma once

#include<Gear/Game.h>

#include"MyRenderPass.h"

class MyGame :public Game
{
public:

	MyGame()
	{
		pushCreateFuture(createRenderPassAsync<MyRenderPass>(&renderPass));

		scheduleAllTasks();
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
		beginRenderPass(renderPass);

		scheduleAllTasks();
	}

	MyRenderPass* renderPass;
	
};