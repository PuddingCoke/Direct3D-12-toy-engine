#include<Gear/Core/RenderPass.h>

std::future<void> RenderPass::getPassResult()
{
	return std::async(std::launch::async, [&]
		{
			begin();

			recordCommand();

			end();
		});
}

RenderPass::RenderPass() :
	renderCMD(new CommandList(D3D12_COMMAND_LIST_TYPE_DIRECT)),
	transitionCMD(new CommandList(D3D12_COMMAND_LIST_TYPE_DIRECT))
{
}

RenderPass::~RenderPass()
{
	if (renderCMD)
	{
		delete renderCMD;
	}

	if (transitionCMD)
	{
		delete transitionCMD;
	}

	for (std::vector<Resource*>& transientPool : transientResources)
	{
		if (transientPool.size())
		{
			for (Resource* res : transientPool)
			{
				delete res;
			}
		}
	}
}

void RenderPass::begin()
{
	for (Resource* res : transientResources[Graphics::getFrameIndex()])
	{
		delete res;
	}

	transientResources[Graphics::getFrameIndex()].clear();

	renderCMD->reset();

	renderCMD->setDescriptorHeap(GlobalDescriptorHeap::getResourceHeap(), GlobalDescriptorHeap::getSamplerHeap());

	renderCMD->setGraphicsRootSignature(GlobalRootSignature::getGraphicsRootSignature());

	renderCMD->setComputeRootSignature(GlobalRootSignature::getComputeRootSignature());
}

void RenderPass::end()
{
	renderCMD->get()->Close();
}

void RenderPass::updateReferredResStates()
{
	for (Resource* res : referredResources)
	{
		res->updateGlobalStates();

		if (res->isSharedResource())
		{
			res->resetInternalStates();
		}
	}

	referredResources.clear();
}

void RenderPass::flushTransitionResources()
{
	for (Buffer* buff : transitionBuffers)
	{
		buff->transition(transitionBarriers, pendingBufferBarrier);
	}

	transitionBuffers.clear();

	for (Texture* tex : transitionTextures)
	{
		tex->transition(transitionBarriers, pendingTextureBarrier);
	}

	transitionTextures.clear();

	renderCMD->get()->ResourceBarrier(transitionBarriers.size(), transitionBarriers.data());

	transitionBarriers.clear();
}
