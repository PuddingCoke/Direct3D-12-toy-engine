#include<Gear/Core/RenderPass.h>

std::future<void> RenderPass::getPassResult()
{
	return std::async(std::launch::async, [&]
		{
			setFixedStates();

			recordCommand();
		});
}

void RenderPass::setFixedStates()
{
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
