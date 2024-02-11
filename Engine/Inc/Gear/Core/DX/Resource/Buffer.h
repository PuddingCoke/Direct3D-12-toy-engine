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

	Buffer(const UINT size, const bool stateTracking, const bool cpuWritable, const void* const data, ID3D12GraphicsCommandList6* commandList, std::vector<Resource*>& transientResourcePool);

	Buffer(Buffer&);

	Buffer(const Buffer&) = delete;

	void operator=(const Buffer&) = delete;

	void update(const void* const data, const UINT dataSize) const;

	virtual ~Buffer();

	void updateGlobalStates() override;

	void resetInternalStates() override;

	void resetTransitionStates() override;

	void transition(std::vector<D3D12_RESOURCE_BARRIER>& transitionBarriers, std::vector<PendingBufferBarrier>& pendingBarriers);

protected:

	UINT transitionState;

private:

	friend class RenderEngine;

	friend class RenderPass;

	UploadHeap* uploadHeaps[Graphics::FrameBufferCount];

	UINT uploadHeapIndex;

	std::shared_ptr<UINT> globalState;

	UINT internalState;

};

#endif // !_BUFFER_H_

