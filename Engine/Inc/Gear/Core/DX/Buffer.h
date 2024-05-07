#pragma once

#ifndef _BUFFER_H_
#define _BUFFER_H_

#include"Resource.h"

#include<vector>

class Buffer;

struct PendingBufferBarrier
{
	Buffer* buffer;

	UINT afterState;
};

class Buffer :public Resource
{
public:

	Buffer(const UINT size, const bool stateTracking, const D3D12_RESOURCE_FLAGS resFlags, const D3D12_RESOURCE_STATES initialState = D3D12_RESOURCE_STATE_COPY_DEST);

	Buffer(const ComPtr<ID3D12Resource>& buffer, const bool stateTracking, const UINT initialState);

	Buffer(Buffer&);

	Buffer(const Buffer&) = delete;

	void operator=(const Buffer&) = delete;

	virtual ~Buffer();

	void updateGlobalStates() override;

	void resetInternalStates() override;

	void resetTransitionStates() override;

	void transition(std::vector<D3D12_RESOURCE_BARRIER>& transitionBarriers, std::vector<PendingBufferBarrier>& pendingBarriers);

	void setState(const UINT state);

	UINT getState() const;

private:

	friend class RenderEngine;

	std::shared_ptr<UINT> globalState;

	UINT internalState;

	UINT transitionState;

};

#endif // !_BUFFER_H_

