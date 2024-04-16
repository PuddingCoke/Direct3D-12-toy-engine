#pragma once

#include<Gear/Game.h>

#include<Gear/Camera/FPSCamera.h>

#include"MyRenderPass.h"

class MyGame :public Game
{
public:

	MyGame():
		camera({ 1024,100,1024 }, { 0,-0.2f,-1.f }, { 0,1,0 }, 100),
		renderPass(new MyRenderPass())
	{
		camera.registerEvent();

		Camera::setProj(Math::pi / 4.f, Graphics::getAspectRatio(), 1.f, 4096.f);
	}

	~MyGame()
	{
		delete renderPass;
	}

	void update(const float dt) override
	{
		camera.applyInput(dt);
	}

	void render()
	{
		renderPass->beginRenderPass();

		RenderEngine::get()->submitRenderPass(renderPass);
	}

	MyRenderPass* renderPass;
	
	FPSCamera camera;
};