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

	Buffer(const UINT size, const bool stateTracking, const bool cpuWritable);

	Buffer(Buffer&);

	Buffer(const Buffer&) = delete;

	void operator=(const Buffer&) = delete;

	void update(const void* const data, const UINT dataSize) const;

	virtual ~Buffer();

	void updateGlobalStates() override;

	void resetInternalStates() override;

	void resetTransitionStates() override;

	void pushBarriersAndStateChanging(std::vector<D3D12_RESOURCE_BARRIER>& transitionBarriers, std::vector<PendingBufferBarrier>& pendingBarriers);

private:

	friend class CopyEngine;

	friend class RenderEngine;

	UploadHeap* uploadHeaps[Graphics::FrameBufferCount];

	UINT uploadHeapIndex;

	std::shared_ptr<UINT> globalState;

	UINT internalState;

	UINT transitionState;

};

#endif // !_BUFFER_H_

