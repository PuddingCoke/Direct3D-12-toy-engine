#pragma once

#ifndef _BUFFER_H_
#define _BUFFER_H_

#include"Resource.h"

#include<vector>

class Buffer;

struct PendingBufferBarrier
{
	Buffer* buffer;

	uint32_t afterState;
};

class Buffer :public Resource
{
public:

	Buffer() = delete;

	Buffer(const Buffer&) = delete;

	void operator=(const Buffer&) = delete;

	Buffer(const uint64_t size, const bool stateTracking, const D3D12_RESOURCE_FLAGS resFlags, const uint32_t initialState = D3D12_RESOURCE_STATE_COPY_DEST);

	Buffer(const ComPtr<ID3D12Resource>& buffer, const bool stateTracking, const uint32_t initialState);

	Buffer(Buffer&);

	virtual ~Buffer();

	void updateGlobalStates() override;

	void resetInternalStates() override;

	void resetTransitionStates() override;

	void transition(std::vector<D3D12_RESOURCE_BARRIER>& transitionBarriers, std::vector<PendingBufferBarrier>& pendingBarriers);

	void setState(const uint32_t state);

	uint32_t getState() const;

private:

	friend class RenderEngine;

	std::shared_ptr<uint32_t> globalState;

	uint32_t internalState;

	uint32_t transitionState;

};

#endif // !_BUFFER_H_