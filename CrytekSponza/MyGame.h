#pragma once

#include<Gear/Camera/FPSCamera.h>

#include<Gear/Game.h>

#include"MyRenderTask.h"

//generic 26%
//all 23.5%

class MyGame :public Game
{
public:

	FPSCamera camera;

	MyGame():
		camera({ 0.f,20.f,0.f }, { 1.0f,0.f,0.f }, { 0.f,1.f,0.f }, 100.f)
	{
		Camera::setProj(Math::pi / 4.f, Graphics::getAspectRatio(), 1.f, 512.f);

		pushCreateFuture(createRenderTaskAsync(&renderTask));

		scheduleAllTasks();
	}

	~MyGame()
	{
		delete renderTask;
	}

	void update(const float dt) override
	{
		camera.applyInput(dt);
	}

	void render()
	{
		beginRenderTask(renderTask);

		scheduleAllTasks();
	}

	MyRenderTask* renderTask;
	
};