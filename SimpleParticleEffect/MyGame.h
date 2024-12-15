#pragma once

#include<Gear/Game.h>

#include<Gear/Camera/OrbitCamera.h>

#include"MyRenderPass.h"

class MyGame :public Game
{
public:

	MyGame():
		camera({ 4,4,-11 }, { -1,-1,-2 }, 2.f)
	{
		Camera::setProj(Math::pi / 4.f, Graphics::getAspectRatio(), 0.01f, 512.f);

		pushCreateFuture(createRenderPassAsync<MyRenderPass>(&renderPass));

		scheduleAllTasks();
	}

	~MyGame()
	{
		delete renderPass;
	}

	void update(const float dt) override
	{
		camera.rotateX(dt);

		camera.applyInput(dt);
	}

	void render()
	{
		beginRenderPass(renderPass);

		scheduleAllTasks();
	}

	MyRenderPass* renderPass;
	
	OrbitCamera camera;

};