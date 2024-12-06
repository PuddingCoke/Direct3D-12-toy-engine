#pragma once

#ifndef _GAME_H_
#define _GAME_H_

#include<Gear/Core/RenderPass.h>

class Game
{
public:

	Game();

	virtual ~Game();

	virtual void imGUICall();

	void operator=(const Game&) = delete;

	virtual void update(const float dt) = 0;

	virtual void render() = 0;

	void submitRenderPass(RenderPass* const pass);

};

#endif // !_GAME_H_
