#include"ResourceStateTracker.h"

void Gear::Core::D3D12Core::ResourceStateTracker::solvePendingBarriers(std::vector<D3D12_RESOURCE_BARRIER>& barriers)
{
	if (pendingBufferBarrier.size())
	{
		for (const Resource::D3D12Resource::PendingBufferBarrier& pendingBarrier : pendingBufferBarrier)
		{
			pendingBarrier.buffer->solvePendingBarrier(barriers, pendingBarrier.afterState);
		}

		pendingBufferBarrier.clear();
	}

	if (pendingTextureBarrier.size())
	{
		for (const Resource::D3D12Resource::PendingTextureBarrier& pendingBarrier : pendingTextureBarrier)
		{
			pendingBarrier.texture->solvePendingBarrier(barriers, pendingBarrier.mipSlice, pendingBarrier.afterState);
		}

		pendingTextureBarrier.clear();
	}
}

void Gear::Core::D3D12Core::ResourceStateTracker::updateReferredSharedResourceStates()
{
	for (Resource::D3D12Resource::D3D12ResourceBase* const res : referredResources)
	{
		res->updateGlobalStates();

		res->resetInternalStates();

		res->popFromReferredList();
	}

	referredResources.clear();
}

void Gear::Core::D3D12Core::ResourceStateTracker::transitionResources(ID3D12GraphicsCommandList6* const commandList)
{
	for (Resource::D3D12Resource::Buffer* const buff : transitionBuffers)
	{
		buff->transition(transitionBarriers, pendingBufferBarrier);

		buff->popFromTrackingList();
	}

	transitionBuffers.clear();

	for (Resource::D3D12Resource::Texture* const tex : transitionTextures)
	{
		tex->transition(transitionBarriers, pendingTextureBarrier);

		tex->popFromTrackingList();
	}

	transitionTextures.clear();

	if (transitionBarriers.size())
	{
		commandList->ResourceBarrier(static_cast<uint32_t>(transitionBarriers.size()), transitionBarriers.data());

		transitionBarriers.clear();
	}
}

void Gear::Core::D3D12Core::ResourceStateTracker::pushResourceTrackList(Resource::D3D12Resource::Texture* const texture)
{
	texture->pushToReferredList(referredResources);

	texture->pushToTrackingList(transitionTextures);
}

void Gear::Core::D3D12Core::ResourceStateTracker::pushResourceTrackList(Resource::D3D12Resource::Buffer* const buffer)
{
	buffer->pushToReferredList(referredResources);

	buffer->pushToTrackingList(transitionBuffers);
}