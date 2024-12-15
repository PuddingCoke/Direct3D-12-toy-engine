#include<Gear/Core/RenderPass.h>

RenderPass::RenderPass() :
	context(new GraphicsContext()),
	resManager(new ResourceManager(context)),
	task(std::async(std::launch::async, [this]() {return context->getCommandList(); }))
{
	context->begin();
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
}

void RenderPass::beginRenderPass()
{
	task = std::async(std::launch::async, [this]
		{
			resManager->cleanTransientResources();

			context->begin();

			recordCommand();

			//main render thread will solve pending barriers and close commandList
			return context->getCommandList();
		});
}

CommandList* RenderPass::getRenderPassResult()
{
	return task.get();
}

void RenderPass::imGUICall()
{

}

void RenderPass::blit(TextureRenderView* const texture) const
{
	context->setPipelineState(PipelineState::get()->fullScreenBlitState.Get());

	context->setDefRenderTarget();

	context->setViewportSimple(Graphics::getWidth(), Graphics::getHeight());

	context->setTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	context->setPSConstants({ texture->getAllSRVIndex() }, 0);

	context->transitionResources();

	context->draw(3, 1, 0, 0);
}
