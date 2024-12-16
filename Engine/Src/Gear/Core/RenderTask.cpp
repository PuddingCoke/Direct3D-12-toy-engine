#include<Gear/Core/RenderTask.h>

RenderTask::RenderTask() :
	context(new GraphicsContext()),
	resManager(new ResourceManager(context)),
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

	if (context)
	{
		delete context;
	}

	if (resManager)
	{
		delete resManager;
	}
}

void RenderTask::beginTask()
{
	std::lock_guard<std::mutex> lockGuard(taskMutex);

	taskCompleted = false;

	taskCondition.notify_one();
}

void RenderTask::waitTask()
{
	std::unique_lock<std::mutex> lock(taskMutex);

	taskCondition.wait(lock, [this]() {return taskCompleted; });
}

CommandList* RenderTask::getCommandList()
{
	return context->getCommandList();
}

void RenderTask::imGUICall()
{

}

void RenderTask::blit(TextureRenderView* const texture) const
{
	context->setPipelineState(PipelineState::get()->fullScreenBlitState.Get());

	context->setDefRenderTarget();

	context->setViewportSimple(Graphics::getWidth(), Graphics::getHeight());

	context->setTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	context->setPSConstants({ texture->getAllSRVIndex() }, 0);

	context->transitionResources();

	context->draw(3, 1, 0, 0);
}

void RenderTask::workerLoop()
{
	while (true)
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

		taskCondition.notify_one();
	}
}
