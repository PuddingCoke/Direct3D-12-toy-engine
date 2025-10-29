#pragma once

#ifndef _GEAR_CORE_RENDERTASK_H_
#define _GEAR_CORE_RENDERTASK_H_

#include<Gear/Core/RenderEngine.h>

#include<Gear/Core/ResourceManager.h>

#include<future>

namespace Gear
{
	namespace Core
	{
		class RenderTask
		{
		public:

			RenderTask(const RenderTask&) = delete;

			void operator=(const RenderTask&) = delete;

			RenderTask();

			virtual ~RenderTask();

			void beginTask();

			void waitTask();

			D3D12Core::CommandList* getCommandList() const;

			virtual void imGUICall();

		protected:

			//把纹理绘制到后备缓冲上
			void blit(Resource::TextureRenderView* const texture) const;

			virtual void recordCommand() = 0;

			ResourceManager* const resManager;

			GraphicsContext* const context;

		private:

			void workerLoop();

			bool taskCompleted;

			bool isRunning;

			std::mutex taskMutex;

			std::condition_variable taskCondition;

			std::thread workerThread;

		};
	}
}

template <typename First, typename... Rest>
std::future<void> createRenderTaskAsync(const First& first, const Rest&... args)
{
	using RenderTaskType = std::remove_pointer_t<std::remove_pointer_t<First>>;

	return std::async(std::launch::async, [=]()
		{
			*first = new RenderTaskType(args...);

			Gear::Core::RenderEngine::submitCommandList((*first)->getCommandList());
		});
}

#endif // !_GEAR_CORE_RENDERTASK_H_