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
	renderCMD->Reset();

	/*
	* GraphicsRootSignature
	* ComputeRootSignature
	* DescriptorHeap
	*/
}

void RenderPass::updateRefResStates()
{
	for (Resource* res : refResources)
	{
		res->updateGlobalStates();
		res->resetInternalStates();
	}

	refResources.clear();
}
