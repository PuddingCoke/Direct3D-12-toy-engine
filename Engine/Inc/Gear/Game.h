#pragma once

#ifndef _GAME_H_
#define _GAME_H_

#include<Gear/Core/RenderTask.h>

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

	void beginRenderTask(RenderTask* const renderTask);

	void pushCreateFuture(std::future<void>&& createFuture);

	void scheduleAllTasks();

	std::queue<RenderTask*> recordQueue;

	std::queue<std::future<void>> createQueue;

};

#endif // !_GAME_H_