#pragma once

#ifndef _GAME_H_
#define _GAME_H_

#include<Gear/Core/RenderPass.h>

#include<queue>

class Game
{
public:

	Game();

	virtual ~Game();

	virtual void imGUICall();

	void operator=(const Game&) = delete;

	virtual void update(const float dt) = 0;

	virtual void render() = 0;

	void beginRenderPass(RenderPass* const renderPass);

	void pushCreateFuture(std::future<void>&& createFuture);

	void scheduleAllTasks();

	std::queue<RenderPass*> recordQueue;

	std::queue<std::future<void>> createQueue;

};

#endif // !_GAME_H_
