#include<Gear/Core/RenderEngine.h>

void RenderEngine::submitRenderPass(RenderPass* const pass)
{
	std::vector<D3D12_RESOURCE_BARRIER> barriers;

	for (UINT bufferIdx = 0; bufferIdx < pass->pendingBufferBarrier.size(); bufferIdx++)
	{
		PendingBufferBarrier pendingBarrier = pass->pendingBufferBarrier[bufferIdx];

		D3D12_RESOURCE_BARRIER barrier = {};
		barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		barrier.Transition.pResource = pendingBarrier.buffer->getResource();

		if ((*(pendingBarrier.buffer->globalState)) != pendingBarrier.afterState)
		{
			barrier.Transition.StateBefore = static_cast<D3D12_RESOURCE_STATES>(*(pendingBarrier.buffer->globalState));
			barrier.Transition.StateAfter = static_cast<D3D12_RESOURCE_STATES>(pendingBarrier.afterState);
			barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

			barriers.push_back(barrier);
		}
	}

	pass->pendingBufferBarrier.clear();

	for (UINT textureIdx = 0; textureIdx < pass->pendingTextureBarrier.size(); textureIdx++)
	{
		PendingTextureBarrier pendingBarrier = pass->pendingTextureBarrier[textureIdx];

		D3D12_RESOURCE_BARRIER barrier = {};
		barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		barrier.Transition.pResource = pendingBarrier.texture->getResource();

		if (pendingBarrier.mipSlice == D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES)
		{
			if (pendingBarrier.texture->mipLevels > 1)
			{
				bool uniformState = true;

				if ((*(pendingBarrier.texture->globalState)).allState == D3D12_RESOURCE_STATE_UNKNOWN)
				{
					UINT tempState = (*(pendingBarrier.texture->globalState)).mipLevelStates[0];

					for (UINT mipSlice = 0; mipSlice < pendingBarrier.texture->mipLevels; mipSlice++)
					{
						if ((*(pendingBarrier.texture->globalState)).mipLevelStates[mipSlice] != tempState)
						{
							uniformState = false;
							break;
						}
					}
				}

				if (uniformState)
				{
					if ((*(pendingBarrier.texture->globalState)).mipLevelStates[0] != pendingBarrier.afterState)
					{
						barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
						barrier.Transition.StateBefore = static_cast<D3D12_RESOURCE_STATES>((*(pendingBarrier.texture->globalState)).mipLevelStates[0]);
						barrier.Transition.StateAfter = static_cast<D3D12_RESOURCE_STATES>(pendingBarrier.afterState);

						barriers.push_back(barrier);
					}
				}
				else
				{
					for (UINT mipSlice = 0; mipSlice < pendingBarrier.texture->mipLevels; mipSlice++)
					{
						if ((*(pendingBarrier.texture->globalState)).mipLevelStates[mipSlice] != pendingBarrier.afterState)
						{
							for (UINT arraySlice = 0; arraySlice < pendingBarrier.texture->arraySize; arraySlice++)
							{
								barrier.Transition.Subresource = D3D12CalcSubresource(mipSlice, arraySlice, 0, pendingBarrier.texture->mipLevels, pendingBarrier.texture->arraySize);
								barrier.Transition.StateBefore = static_cast<D3D12_RESOURCE_STATES>((*(pendingBarrier.texture->globalState)).mipLevelStates[mipSlice]);
								barrier.Transition.StateAfter = static_cast<D3D12_RESOURCE_STATES>(pendingBarrier.afterState);

								barriers.push_back(barrier);
							}
						}
					}
				}
			}
			else
			{
				if ((*(pendingBarrier.texture->globalState)).allState != pendingBarrier.afterState)
				{
					barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
					barrier.Transition.StateBefore = static_cast<D3D12_RESOURCE_STATES>((*(pendingBarrier.texture->globalState)).allState);
					barrier.Transition.StateAfter = static_cast<D3D12_RESOURCE_STATES>(pendingBarrier.afterState);

					barriers.push_back(barrier);
				}
			}
		}
		else
		{
			if (pendingBarrier.texture->mipLevels > 1)
			{
				if ((*(pendingBarrier.texture->globalState)).mipLevelStates[pendingBarrier.mipSlice] != pendingBarrier.afterState)
				{
					for (UINT arraySlice = 0; arraySlice < pendingBarrier.texture->arraySize; arraySlice++)
					{
						barrier.Transition.Subresource = D3D12CalcSubresource(pendingBarrier.mipSlice, arraySlice, 0, pendingBarrier.texture->mipLevels, pendingBarrier.texture->arraySize);
						barrier.Transition.StateBefore = static_cast<D3D12_RESOURCE_STATES>((*(pendingBarrier.texture->globalState)).mipLevelStates[pendingBarrier.mipSlice]);
						barrier.Transition.StateAfter = static_cast<D3D12_RESOURCE_STATES>(pendingBarrier.afterState);

						barriers.push_back(barrier);
					}
				}
			}
			else
			{
				if ((*(pendingBarrier.texture->globalState)).allState != pendingBarrier.afterState)
				{
					barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
					barrier.Transition.StateBefore = static_cast<D3D12_RESOURCE_STATES>((*(pendingBarrier.texture->globalState)).allState);
					barrier.Transition.StateAfter = static_cast<D3D12_RESOURCE_STATES>(pendingBarrier.afterState);

					barriers.push_back(barrier);
				}
			}
		}
	}

	pass->pendingTextureBarrier.clear();

	if (barriers.size() > 0)
	{
		pass->transitionCMD->Reset();

		pass->transitionCMD->get()->ResourceBarrier(barriers.size(), barriers.data());

		pass->transitionCMD->get()->Close();

		ID3D12CommandList* const commandLists[2] = { pass->transitionCMD->get(),pass->renderCMD->get() };

		commandQueue->ExecuteCommandLists(2, commandLists);
	}
	else
	{
		ID3D12CommandList* const commandLists[] = { pass->renderCMD->get() };

		commandQueue->ExecuteCommandLists(1, commandLists);
	}

	pass->updateRefResStates();
}
