#pragma once

#ifndef _INDEXBUFFER_H_
#define _INDEXBUFFER_H_

#include<Gear/Core/DX/Resource/Buffer.h>

#include"EngineResource.h"

class IndexBuffer :public EngineResource
{
public:

	IndexBuffer(Buffer* const buffer, const DXGI_FORMAT format, const UINT size, const bool cpuWritable);

	IndexBuffer(const IndexBuffer&);

	~IndexBuffer();

	D3D12_INDEX_BUFFER_VIEW getIndexBufferView() const;

	Buffer* getBuffer() const;

private:

	friend class GraphicsContext;

	D3D12_INDEX_BUFFER_VIEW indexBufferView;

	UploadHeap** uploadHeaps;

	UINT uploadHeapIndex;

	Buffer* buffer;

};

#endif // !_INDEXBUFFER_H_
