#include<Gear/Core/RenderPass.h>

void RenderPass::beginRenderPass()
{
	task = std::async(std::launch::async, [this]
		{
			begin();

			recordCommand();

			end();

			return RenderPassResult{ transitionCMD,context->getCommandList() };
		});
}

RenderPassResult RenderPass::getRenderPassResult()
{
	return task.get();
}

RenderPass::RenderPass() :
	context(new GraphicsContext()),
	resManager(new ResourceManager(context)),
	transitionCMD(new CommandList(D3D12_COMMAND_LIST_TYPE_DIRECT)),
	task(std::async(std::launch::async, [this] { return RenderPassResult{ transitionCMD,context->getCommandList() }; }))
{
}

RenderPass::~RenderPass()
{
	if (context)
	{
		delete context;
	}

	if (resManager)
	{
		delete resManager;
	}

	if (transitionCMD)
	{
		delete transitionCMD;
	}
}

void RenderPass::begin()
{
	resManager->cleanTransientResources();

	context->begin();
}

void RenderPass::end() const
{
	context->end();
}
