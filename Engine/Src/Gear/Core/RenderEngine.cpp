#include<Gear/Core/RenderEngine.h>

void RenderEngine::submitRenderPass(RenderPass* const pass)
{
	std::vector<D3D12_RESOURCE_BARRIER> barriers;

	pass->transitionCMD->Reset();

	for (UINT i = 0; i < pass->pendingBufferBarrier.size(); i++)
	{
		D3D12_RESOURCE_BARRIER barrier = {};
		barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;

		if ((*(pass->pendingBufferBarrier[i].buffer->globalState)) != pass->pendingBufferBarrier[i].afterState)
		{
			barrier.Transition.pResource = pass->pendingBufferBarrier[i].buffer->getResource();
			barrier.Transition.StateBefore = static_cast<D3D12_RESOURCE_STATES>(*(pass->pendingBufferBarrier[i].buffer->globalState));
			barrier.Transition.StateAfter = static_cast<D3D12_RESOURCE_STATES>(pass->pendingBufferBarrier[i].afterState);
			barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

			barriers.push_back(barrier);
		}

	}

	pass->pendingBufferBarrier.clear();

	for (UINT i = 0; i < pass->pendingTextureBarrier.size(); i++)
	{
		D3D12_RESOURCE_BARRIER barrier = {};
		barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		barrier.Transition.pResource = pass->pendingTextureBarrier[i].texture->getResource();

		if (pass->pendingTextureBarrier[i].mipSlice == D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES)
		{
			if (pass->pendingTextureBarrier[i].texture->mipLevels > 1)
			{
				bool uniformState = true;

				if ((*(pass->pendingTextureBarrier[i].texture->globalState)).allState == D3D12_RESOURCE_STATE_UNKNOWN)
				{
					UINT tempState = (*(pass->pendingTextureBarrier[i].texture->globalState)).mipLevelStates[0];

					for (UINT j = 0; j < pass->pendingTextureBarrier[i].texture->mipLevels; j++)
					{
						if ((*(pass->pendingTextureBarrier[i].texture->globalState)).mipLevelStates[j] != tempState)
						{
							uniformState = false;
							break;
						}
					}
				}

				if (uniformState)
				{
					if ((*(pass->pendingTextureBarrier[i].texture->globalState)).mipLevelStates[0] != pass->pendingTextureBarrier[i].afterState)
					{
						barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
						barrier.Transition.StateBefore = static_cast<D3D12_RESOURCE_STATES>((*(pass->pendingTextureBarrier[i].texture->globalState)).mipLevelStates[0]);
						barrier.Transition.StateAfter = static_cast<D3D12_RESOURCE_STATES>(pass->pendingTextureBarrier[i].afterState);

						barriers.push_back(barrier);
					}
				}
				else
				{
					for (UINT j = 0; j < pass->pendingTextureBarrier[i].texture->mipLevels; j++)
					{
						if ((*(pass->pendingTextureBarrier[i].texture->globalState)).mipLevelStates[j] != pass->pendingTextureBarrier[i].afterState)
						{
							for (UINT k = 0; k < pass->pendingTextureBarrier[i].texture->arraySize; k++)
							{
								barrier.Transition.Subresource = D3D12CalcSubresource(j, k, 0, pass->pendingTextureBarrier[i].texture->mipLevels, pass->pendingTextureBarrier[i].texture->arraySize);
								barrier.Transition.StateBefore = static_cast<D3D12_RESOURCE_STATES>((*(pass->pendingTextureBarrier[i].texture->globalState)).mipLevelStates[j]);
								barrier.Transition.StateAfter = static_cast<D3D12_RESOURCE_STATES>(pass->pendingTextureBarrier[i].afterState);

								barriers.push_back(barrier);
							}
						}
					}
				}

			}
			else
			{
				if ((*(pass->pendingTextureBarrier[i].texture->globalState)).allState != pass->pendingTextureBarrier[i].afterState)
				{
					barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
					barrier.Transition.StateBefore = static_cast<D3D12_RESOURCE_STATES>((*(pass->pendingTextureBarrier[i].texture->globalState)).allState);
					barrier.Transition.StateAfter = static_cast<D3D12_RESOURCE_STATES>(pass->pendingTextureBarrier[i].afterState);

					barriers.push_back(barrier);
				}
			}
		}
		else
		{
			if ((*(pass->pendingTextureBarrier[i].texture->globalState)).mipLevelStates[pass->pendingTextureBarrier[i].mipSlice] != pass->pendingTextureBarrier[i].afterState)
			{
				for (UINT j = 0; j < pass->pendingTextureBarrier[i].texture->arraySize; j++)
				{
					barrier.Transition.Subresource = D3D12CalcSubresource(pass->pendingTextureBarrier[i].mipSlice, j, 0, pass->pendingTextureBarrier[i].texture->mipLevels, pass->pendingTextureBarrier[i].texture->arraySize);
					barrier.Transition.StateBefore = static_cast<D3D12_RESOURCE_STATES>((*(pass->pendingTextureBarrier[i].texture->globalState)).mipLevelStates[pass->pendingTextureBarrier[i].mipSlice]);
					barrier.Transition.StateAfter = static_cast<D3D12_RESOURCE_STATES>(pass->pendingTextureBarrier[i].afterState);

					barriers.push_back(barrier);
				}
			}
		}

	}

	pass->pendingTextureBarrier.clear();

	pass->transitionCMD->get()->ResourceBarrier(barriers.size(), barriers.data());

	pass->transitionCMD->get()->Close();

	ID3D12CommandList* const commandLists[2] = { pass->transitionCMD->get(),pass->renderCMD->get() };

	commandQueue->ExecuteCommandLists(2, commandLists);

	pass->updateRefResStates();
}
