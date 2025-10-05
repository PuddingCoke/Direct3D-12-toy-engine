#pragma once

#include<Gear/Game.h>

#include<Gear/Camera/FPSCamera.h>

#include"MyRenderTask.h"

class MyGame :public Game
{
public:

	MyGame() :
		camera({ 0.f,100.f,0.f }, { 1.f * sinf(0.63f),-0.1f,1.f * cosf(0.63f) }, { 0.f,1.f,0.f }, 100.f)
	{
		Camera::setProj(Math::pi / 3.f, Graphics::getAspectRatio(), 1.f, 5000.f);

		pushCreateFuture(createRenderTaskAsync(&renderTask, &camera));

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

	FPSCamera camera;

};