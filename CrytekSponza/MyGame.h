#pragma once

#include<Gear/Camera/FPSCamera.h>

#include<Gear/Game.h>

#include"MyRenderPass.h"

//generic 26%
//all 23.5%

class MyGame :public Game
{
public:

	FPSCamera camera;

	MyGame():
		camera({ 0.f,20.f,0.f }, { 1.0f,0.f,0.f }, { 0.f,1.f,0.f }, 100.f),
		renderPass(new MyRenderPass())
	{
		Camera::setProj(Math::pi / 4.f, Graphics::getAspectRatio(), 1.f, 512.f);
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
	
};