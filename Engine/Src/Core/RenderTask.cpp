#include<Gear/Core/RenderTask.h>

#include<Gear/Core/GlobalEffect/BackBufferBlitEffect.h>

Core::RenderTask::RenderTask() :
	resManager(new ResourceManager()),
	context(resManager->getGraphicsContext()),
	taskCompleted(true),
	isRunning(true),
	workerThread(std::thread(&Core::RenderTask::workerLoop, this))
{
	context->begin();
}

Core::RenderTask::~RenderTask()
{
	isRunning = false;

	beginTask();

	if (workerThread.joinable())
	{
		workerThread.join();
	}

	if (resManager)
	{
		delete resManager;
	}
}

void Core::RenderTask::beginTask()
{
	{
		std::lock_guard<std::mutex> lockGuard(taskMutex);

		taskCompleted = false;
	}

	taskCondition.notify_one();
}

void Core::RenderTask::waitTask()
{
	std::unique_lock<std::mutex> lock(taskMutex);

	taskCondition.wait(lock, [this]() {return taskCompleted; });
}

Core::D3D12Core::CommandList* Core::RenderTask::getCommandList() const
{
	return context->getCommandList();
}

void Core::RenderTask::imGUICall()
{

}

void Core::RenderTask::blit(Resource::TextureRenderView* const texture) const
{
	GlobalEffect::BackBufferBlitEffect::process(context, texture);
}

void Core::RenderTask::workerLoop()
{
	while (true)
	{
		{
			std::unique_lock<std::mutex> lock(taskMutex);

			taskCondition.wait(lock, [this]() {return !taskCompleted; });

			if (!isRunning)
			{
				break;
			}

			resManager->cleanTransientResources();

			context->begin();

			recordCommand();

			taskCompleted = true;
		}

		taskCondition.notify_one();
	}
}
