#include<Gear/Core/DX/Resource/Buffer.h>

Buffer::Buffer(const UINT size, const bool stateTracking, const bool cpuWritable, const void* const data, ID3D12GraphicsCommandList6* commandList, std::vector<Resource*>& transientResourcePool, const D3D12_RESOURCE_STATES finalState) :
	Resource(CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE, CD3DX12_RESOURCE_DESC::Buffer(size), cpuWritable | stateTracking, D3D12_RESOURCE_STATE_COPY_DEST, nullptr),
	uploadHeaps{},
	uploadHeapIndex(0),
	globalState(std::make_shared<UINT>(D3D12_RESOURCE_STATE_COPY_DEST)),
	internalState(D3D12_RESOURCE_STATE_COPY_DEST),
	transitionState(D3D12_RESOURCE_STATE_UNKNOWN)
{
	if (data)
	{
		UploadHeap* uploadHeap = new UploadHeap(size);

		transientResourcePool.push_back(uploadHeap);

		uploadHeap->update(data, size);

		commandList->CopyResource(getResource(), uploadHeap->getResource());

		if (!stateTracking)
		{
			D3D12_RESOURCE_BARRIER barrier = {};
			barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
			barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
			barrier.Transition.pResource = getResource();
			barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
			barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
			barrier.Transition.StateAfter = finalState;

			commandList->ResourceBarrier(1, &barrier);

			*globalState = finalState;
			internalState = finalState;
		}
	}

	if (cpuWritable)
	{
		for (UINT i = 0; i < Graphics::FrameBufferCount; i++)
		{
			uploadHeaps[i] = new UploadHeap(size, D3D12_HEAP_FLAG_NONE);
		}
	}
}

Buffer::Buffer(Buffer& buff) :
	Resource(buff),
	uploadHeaps{},
	uploadHeapIndex(buff.uploadHeapIndex),
	globalState(buff.globalState),
	internalState(D3D12_RESOURCE_STATE_UNKNOWN),
	transitionState(D3D12_RESOURCE_STATE_UNKNOWN)
{
}

void Buffer::update(const void* const data, const UINT dataSize) const
{
	uploadHeaps[uploadHeapIndex]->update(data, dataSize);
}

Buffer::~Buffer()
{
	for (UINT i = 0; i < Graphics::FrameBufferCount; i++)
	{
		if (uploadHeaps[i])
		{
			delete uploadHeaps[i];
		}
	}
}

void Buffer::updateGlobalStates()
{
	if (internalState != D3D12_RESOURCE_STATE_UNKNOWN)
	{
		*globalState = internalState;
	}
}

void Buffer::resetInternalStates()
{
	internalState = D3D12_RESOURCE_STATE_UNKNOWN;
}

void Buffer::resetTransitionStates()
{
	transitionState = D3D12_RESOURCE_STATE_UNKNOWN;
}

void Buffer::transition(std::vector<D3D12_RESOURCE_BARRIER>& transitionBarriers, std::vector<PendingBufferBarrier>& pendingBarriers)
{
	if (internalState == D3D12_RESOURCE_STATE_UNKNOWN)
	{
		PendingBufferBarrier barrier = {};
		barrier.buffer = this;
		barrier.afterState = transitionState;

		pendingBarriers.push_back(barrier);

		internalState = transitionState;
	}
	else
	{
		if (internalState != transitionState)
		{
			D3D12_RESOURCE_BARRIER barrier = {};
			barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
			barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
			barrier.Transition.pResource = getResource();
			barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
			barrier.Transition.StateBefore = static_cast<D3D12_RESOURCE_STATES>(internalState);
			barrier.Transition.StateAfter = static_cast<D3D12_RESOURCE_STATES>(transitionState);

			transitionBarriers.push_back(barrier);

			internalState = transitionState;
		}
	}

	resetTransitionStates();
}
