#pragma once

#ifndef _VERTEXBUFFER_H_
#define _VERTEXBUFFER_H_

#include<Gear/Core/DX/Resource/Buffer.h>

#include"EngineResource.h"

class VertexBuffer :public EngineResource
{
public:

	VertexBuffer(Buffer* const buffer, const UINT perVertexSize, const UINT size, const bool cpuWritable);

	VertexBuffer(const VertexBuffer&);

	~VertexBuffer();

	D3D12_VERTEX_BUFFER_VIEW getVertexBufferView() const;

	Buffer* getBuffer() const;

private:

	friend class GraphicsContext;

	D3D12_VERTEX_BUFFER_VIEW vertexBufferView;

	UploadHeap** uploadHeaps;

	UINT uploadHeapIndex;

	Buffer* buffer;
};

#endif // !_VERTEXBUFFER_H_
