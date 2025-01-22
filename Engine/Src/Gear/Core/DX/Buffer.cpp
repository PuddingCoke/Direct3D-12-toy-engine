﻿#include<Gear/Core/DX/Buffer.h>

Buffer::Buffer(const uint64_t size, const bool stateTracking, const D3D12_RESOURCE_FLAGS resFlags, const uint32_t initialState) :
	Resource(CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE, CD3DX12_RESOURCE_DESC::Buffer(size, resFlags), stateTracking, static_cast<D3D12_RESOURCE_STATES>(initialState), nullptr),
	globalState(std::make_shared<uint32_t>(initialState)),
	internalState(initialState),
	transitionState(D3D12_RESOURCE_STATE_UNKNOWN)
{
}

Buffer::Buffer(const ComPtr<ID3D12Resource>& buffer, const bool stateTracking, const uint32_t initialState) :
	Resource(buffer, stateTracking),
	globalState(std::make_shared<uint32_t>(initialState)),
	internalState(initialState),
	transitionState(D3D12_RESOURCE_STATE_UNKNOWN)
{
}

Buffer::Buffer(Buffer& buff) :
	Resource(buff),
	globalState(buff.globalState),
	internalState(D3D12_RESOURCE_STATE_UNKNOWN),
	transitionState(D3D12_RESOURCE_STATE_UNKNOWN)
{
}

Buffer::~Buffer()
{
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
	else if (!bitFlagSubset(internalState, transitionState))
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

	resetTransitionStates();
}

void Buffer::setState(const uint32_t state)
{
	if (transitionState == D3D12_RESOURCE_STATE_UNKNOWN)
	{
		transitionState = state;
	}
	else
	{
		transitionState = transitionState | state;
	}
}

uint32_t Buffer::getState() const
{
	return internalState;
}
