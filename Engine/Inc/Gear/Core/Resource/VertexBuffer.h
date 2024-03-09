#pragma once

#ifndef _VERTEXBUFFER_H_
#define _VERTEXBUFFER_H_

#include<Gear/Core/DX/Resource/Buffer.h>

class VertexBuffer
{
public:

	VertexBuffer(const UINT perVertexSize,const UINT size, const bool cpuWritable, const void* const data, ID3D12GraphicsCommandList6* commandList, std::vector<Resource*>* transientResourcePool);

	VertexBuffer(const VertexBuffer&);

	~VertexBuffer();

	D3D12_VERTEX_BUFFER_VIEW getVertexBufferView() const;

	Buffer* getBuffer();

private:

	friend class GraphicsContext;

	D3D12_VERTEX_BUFFER_VIEW vertexBufferView;

	Buffer* buffer;

	UploadHeap** uploadHeaps;

	UINT uploadHeapIndex;

};

#endif // !_VERTEXBUFFER_H_
