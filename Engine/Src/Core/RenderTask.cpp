#include<Gear/Core/RenderTask.h>

#include<Gear/Core/GlobalEffect/BackBufferBlitEffect.h>

Gear::Core::RenderTask::RenderTask() :
	resManager(new ResourceManager()),
	context(resManager->getGraphicsContext()),
	taskCompleted(true),
	isRunning(true),
	workerThread(std::thread(&Gear::Core::RenderTask::workerLoop, this))
{
	context->begin();
}

Gear::Core::RenderTask::~RenderTask()
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

void Gear::Core::RenderTask::beginTask()
{
	{
		std::lock_guard<std::mutex> lockGuard(taskMutex);

		taskCompleted = false;
	}

	taskCondition.notify_one();
}

void Gear::Core::RenderTask::waitTask()
{
	std::unique_lock<std::mutex> lock(taskMutex);

	taskCondition.wait(lock, [this]() {return taskCompleted; });
}

Gear::Core::D3D12Core::CommandList* Gear::Core::RenderTask::getCommandList() const
{
	return context->getCommandList();
}

void Gear::Core::RenderTask::imGUICall()
{

}

void Gear::Core::RenderTask::blit(Resource::TextureRenderView* const texture) const
{
	GlobalEffect::BackBufferBlitEffect::process(context, texture);
}

void Gear::Core::RenderTask::workerLoop()
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
