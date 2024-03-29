#pragma once

#ifndef _BUFFER_H_
#define _BUFFER_H_

#include<Gear/Core/GraphicsDevice.h>

#include"UploadHeap.h"

class Buffer;

struct PendingBufferBarrier
{
	Buffer* buffer;

	UINT afterState;
};

class Buffer :public Resource
{
public:

	Buffer(const UINT size, const bool stateTracking, const void* const data, ID3D12GraphicsCommandList6* commandList, std::vector<Resource*>* transientResourcePool, const D3D12_RESOURCE_STATES finalState);

	Buffer(Buffer&);

	Buffer(const Buffer&) = delete;

	void operator=(const Buffer&) = delete;

	virtual ~Buffer();

	void updateGlobalStates() override;

	void resetInternalStates() override;

	void resetTransitionStates() override;

	void transition(std::vector<D3D12_RESOURCE_BARRIER>& transitionBarriers, std::vector<PendingBufferBarrier>& pendingBarriers);

protected:

	UINT transitionState;

private:

	friend class RenderEngine;

	friend class GraphicsContext;

	std::shared_ptr<UINT> globalState;

	UINT internalState;

};

#endif // !_BUFFER_H_

