#pragma once

#ifndef _INDEXBUFFER_H_
#define _INDEXBUFFER_H_

#include<Gear/Core/DX/Resource/Buffer.h>

class IndexBuffer
{
public:

	IndexBuffer(const DXGI_FORMAT format, const UINT size, const bool cpuWritable, const void* const data, ID3D12GraphicsCommandList6* commandList, std::vector<Resource*>* transientResourcePool);

	IndexBuffer(const IndexBuffer&);

	~IndexBuffer();

	D3D12_INDEX_BUFFER_VIEW getIndexBufferView() const;

	Buffer* getBuffer() const;

private:

	friend class GraphicsContext;

	D3D12_INDEX_BUFFER_VIEW indexBufferView;

	Buffer* buffer;

	UploadHeap** uploadHeaps;

	UINT uploadHeapIndex;

};

#endif // !_INDEXBUFFER_H_
