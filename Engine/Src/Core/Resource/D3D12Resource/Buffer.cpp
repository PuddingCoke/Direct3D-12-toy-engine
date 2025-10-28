﻿#include<Gear/Core/Resource/D3D12Resource/Buffer.h>

Core::Resource::D3D12Resource::Buffer::Buffer(const uint64_t size, const bool stateTracking, const D3D12_RESOURCE_FLAGS resFlags, const uint32_t initialState) :
	D3D12ResourceBase(CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE, CD3DX12_RESOURCE_DESC::Buffer(size, resFlags), stateTracking, static_cast<D3D12_RESOURCE_STATES>(initialState), nullptr),
	globalState(std::make_shared<uint32_t>(initialState)),
	internalState(initialState),
	transitionState(D3D12_RESOURCE_STATE_UNKNOWN)
{
}

Core::Resource::D3D12Resource::Buffer::Buffer(const ComPtr<ID3D12Resource>& buffer, const bool stateTracking, const uint32_t initialState) :
	D3D12ResourceBase(buffer, stateTracking),
	globalState(std::make_shared<uint32_t>(initialState)),
	internalState(initialState),
	transitionState(D3D12_RESOURCE_STATE_UNKNOWN)
{
}

Core::Resource::D3D12Resource::Buffer::Buffer(Buffer* const buff) :
	D3D12ResourceBase(buff),
	globalState(buff->globalState),
	internalState(D3D12_RESOURCE_STATE_UNKNOWN),
	transitionState(D3D12_RESOURCE_STATE_UNKNOWN)
{
	buff->resetInternalStates();
}

Core::Resource::D3D12Resource::Buffer::~Buffer()
{
}

void Core::Resource::D3D12Resource::Buffer::updateGlobalStates()
{
	if (internalState != D3D12_RESOURCE_STATE_UNKNOWN)
	{
		*globalState = internalState;
	}
}

void Core::Resource::D3D12Resource::Buffer::resetInternalStates()
{
	internalState = D3D12_RESOURCE_STATE_UNKNOWN;
}

void Core::Resource::D3D12Resource::Buffer::resetTransitionStates()
{
	transitionState = D3D12_RESOURCE_STATE_UNKNOWN;
}

void Core::Resource::D3D12Resource::Buffer::transition(std::vector<D3D12_RESOURCE_BARRIER>& transitionBarriers, std::vector<PendingBufferBarrier>& pendingBarriers)
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

void Core::Resource::D3D12Resource::Buffer::solvePendingBarrier(std::vector<D3D12_RESOURCE_BARRIER>& transitionBarriers, const uint32_t targetState)
{
	if (*globalState != targetState)
	{
		D3D12_RESOURCE_BARRIER barrier = {};
		barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		barrier.Transition.pResource = getResource();
		barrier.Transition.StateBefore = static_cast<D3D12_RESOURCE_STATES>(*globalState);
		barrier.Transition.StateAfter = static_cast<D3D12_RESOURCE_STATES>(targetState);
		barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

		transitionBarriers.push_back(barrier);
	}
}

void Core::Resource::D3D12Resource::Buffer::setState(const uint32_t state)
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

uint32_t Core::Resource::D3D12Resource::Buffer::getState() const
{
	return internalState;
}

void Core::Resource::D3D12Resource::Buffer::pushToTrackingList(std::vector<Buffer*>& trackingList)
{
	if (getStateTracking() && !getInTrackingList())
	{
		trackingList.push_back(this);

		D3D12ResourceBase::pushToTrackingList();
	}
}
