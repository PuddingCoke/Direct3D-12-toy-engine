#pragma once

#include<Gear/Game.h>

#include<Gear/Camera/FPSCamera.h>

#include"MyRenderPass.h"

class MyGame :public Game
{
public:

	MyGame() :
		camera({ 0.f,100.f,0.f }, { 0.f,-0.1f,1.f }, { 0.f,1.f,0.f }, 100.f),
		renderPass(new MyRenderPass(&camera))
	{
		Camera::setProj(Math::pi / 4.f, Graphics::getAspectRatio(), 1.f, 5000.f);
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