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

void RenderPass::begin() const
{
	resManager->cleanTransientResources();

	context->begin();
}

void RenderPass::end() const
{
	context->end();
}
