#pragma once

#include<Gear/Game.h>

#include"MyRenderTask.h"

class MyGame :public Game
{
public:

	MyGame()
	{
		pushCreateFuture(createRenderTaskAsync<MyRenderTask>(&renderTask));

		scheduleAllTasks();
	}

	~MyGame()
	{
		delete renderTask;
	}

	void update(const float dt) override
	{

	}

	void render()
	{
		beginRenderTask(renderTask);

		scheduleAllTasks();
	}

	MyRenderTask* renderTask;


};