#pragma once

#ifndef _GEAR_GAME_H_
#define _GEAR_GAME_H_

#include<Gear/Core/RenderTask.h>

#include<queue>

namespace Gear
{
	class Game
	{
	public:

		Game();

		virtual ~Game();

		virtual void imGUICall();

		Game(const Game&) = delete;

		void operator=(const Game&) = delete;

		virtual void update(const float dt) = 0;

		virtual void render() = 0;

		void beginRenderTask(Gear::Core::RenderTask* const renderTask);

		void pushCreateFuture(std::future<void>&& createFuture);

		void scheduleAllTasks();

		std::queue<Gear::Core::RenderTask*> recordQueue;

		std::queue<std::future<void>> createQueue;

	};
}

#endif // !_GEAR_GAME_H_