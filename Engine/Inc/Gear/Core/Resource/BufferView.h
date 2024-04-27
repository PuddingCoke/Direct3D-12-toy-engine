#pragma once

#ifndef _BUFFERVIEW_H_
#define _BUFFERVIEW_H_

#include<Gear/Core/DX/Resource/Buffer.h>

#include<Gear/Utils/Utils.h>

#include"EngineResource.h"

class BufferView :public EngineResource
{
public:

	BufferView(Buffer* const buffer, const UINT structureByteStride, const DXGI_FORMAT format, const UINT size, const bool createSRV, const bool createUAV, const bool createVBV, const bool createIBV, const bool cpuWritable, const bool persistent);

	~BufferView();

	VertexBufferDesc getVertexBuffer();

	IndexBufferDesc getIndexBuffer();

	ShaderResourceDesc getSRVIndex();

	ShaderResourceDesc getUAVIndex();

	void copyDescriptors() override;

	const bool hasSRV;

	const bool hasUAV;

private:

	friend class GraphicsContext;

	UINT srvIndex;

	UINT uavIndex;

	union
	{
		D3D12_VERTEX_BUFFER_VIEW vbv;

		D3D12_INDEX_BUFFER_VIEW ibv;
	};

	UploadHeap** uploadHeaps;

	UINT uploadHeapIndex;

	Buffer* buffer;

};

#endif // !_BUFFERVIEW_H_
