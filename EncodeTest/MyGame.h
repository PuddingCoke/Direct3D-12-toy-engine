#pragma once

#include<Gear/Game.h>

#include"MyRenderTask.h"

class MyGame :public Game
{
public:

	MyGame()
	{
		pushCreateFuture(createRenderTaskAsync(&renderTask));

		scheduleAllTasks();
	}

	~MyGame()
	{
		delete renderTask;
	}

	void update(const float dt) override
	{

	}

	void render() override
	{
		beginRenderTask(renderTask);

		scheduleAllTasks();
	}

private:

	MyRenderTask* renderTask;

};