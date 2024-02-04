#pragma once

#ifndef _BUFFER_H_
#define _BUFFER_H_

#include<Gear/Core/GraphicsDevice.h>

#include"UploadHeap.h"

class Buffer :public Resource
{
public:

	Buffer(const UINT size, const bool stateTracking, const bool cpuWritable);

	Buffer(const Buffer&);

	void operator=(const Buffer&) = delete;

	void update(const void* const data, const UINT dataSize);

	virtual ~Buffer();

	void updateGlobalStates() override;

	void resetInternalStates() override;

private:

	friend class CopyEngine;

	friend class RenderEngine;

	std::shared_ptr<UploadHeap> uploadHeaps;

	UINT uploadHeapIndex;

	std::shared_ptr<UINT> globalState;

	UINT internalState;

};

struct PendingBufferBarrier
{
	Buffer* buffer;

	UINT afterState;
};

#endif // !_BUFFER_H_

