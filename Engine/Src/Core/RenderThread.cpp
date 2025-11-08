#include<Gear/Core/RenderThread.h>

#include<Gear/Core/RenderEngine.h>

#include<Gear/Core/Internal/GlobalDescriptorHeapInternal.h>

Gear::Core::RenderThread::RenderThread(const std::function<void(RenderTask**)>& createFunc) :
	taskInitialized(false), createFunc(createFunc), renderTask(nullptr)
{
	renderThread = std::thread(&RenderThread::workerLoop, this);
}

Gear::Core::RenderThread::~RenderThread()
{
	if (renderThread.joinable())
	{
		renderThread.join();
	}
}

void Gear::Core::RenderThread::waitTaskInitialized()
{
	std::unique_lock<std::mutex> lock(taskMutex);

	taskCondition.wait(lock, [this]() {return taskInitialized; });
}

void Gear::Core::RenderThread::workerLoop()
{
	//申请每个渲染线程的 staging resource heap render target heap depth stencil heap
	GlobalDescriptorHeap::Internal::initializeLocalDescriptorHeaps();

	//开始创建RenderTask
	{
		std::lock_guard<std::mutex> lockGuard(taskMutex);

		createFunc(&renderTask);

		renderTask->renderThread = this;

		RenderEngine::submitCommandList(renderTask->getCommandList());

		taskInitialized = true;
	}

	//通知主渲染线程子渲染线程创建完毕
	taskCondition.notify_one();

	//进入工作循环
	//由主渲染线程来调度
	renderTask->workerLoop();

	//释放子渲染线程申请的资源
	GlobalDescriptorHeap::Internal::releaseLocalDescriptorHeaps();
}