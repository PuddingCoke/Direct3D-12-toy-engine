#include<Gear/Core/RenderPass.h>

std::future<void> RenderPass::getPassResult()
{
	return std::async(std::launch::async, [&]
		{
			begin();

			recordCommand();
		});
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

void RenderPass::updateRefResStates()
{
	for (Resource* res : referredResources)
	{
		res->updateGlobalStates();
		res->resetInternalStates();
	}

	referredResources.clear();
}
