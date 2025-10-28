#include<Gear/Core/RenderTask.h>

#include<Gear/Core/GlobalEffect/BackBufferBlitEffect.h>

RenderTask::RenderTask() :
	resManager(new ResourceManager()),
	context(resManager->getGraphicsContext()),
	taskCompleted(true),
	isRunning(true),
	workerThread(std::thread(&RenderTask::workerLoop, this))
{
	context->begin();
}

RenderTask::~RenderTask()
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

void RenderTask::beginTask()
{
	{
		std::lock_guard<std::mutex> lockGuard(taskMutex);

		taskCompleted = false;
	}

	taskCondition.notify_one();
}

void RenderTask::waitTask()
{
	std::unique_lock<std::mutex> lock(taskMutex);

	taskCondition.wait(lock, [this]() {return taskCompleted; });
}

CommandList* RenderTask::getCommandList() const
{
	return context->getCommandList();
}

void RenderTask::imGUICall()
{

}

void RenderTask::blit(TextureRenderView* const texture) const
{
	Core::GlobalEffect::BackBufferBlitEffect::process(context, texture);
}

void RenderTask::workerLoop()
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
