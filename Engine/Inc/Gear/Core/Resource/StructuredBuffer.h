#pragma once

#ifndef _STRUCTUREDBUFFER_H_
#define _STRUCTUREDBUFFER_H_

#include<Gear/Core/DX/Resource/Buffer.h>

#include<Gear/Core/DX/Resource/UploadHeap.h>

#include"EngineResource.h"

class StructuredBuffer :public EngineResource
{
public:

	StructuredBuffer(Buffer* const buffer, const UINT structureByteStride, const UINT size, const bool cpuWritable, const bool persistent);

	~StructuredBuffer();

	ShaderResourceDesc getBufferIndex();

	Buffer* getBuffer() const;

	void copyDescriptors() override;

private:

	friend class GraphcisContext;

	UINT bufferIndex;

	UploadHeap** uploadHeaps;

	UINT uploadHeapIndex;

	Buffer* buffer;

};

#endif // !_STRUCTUREDBUFFER_H_
