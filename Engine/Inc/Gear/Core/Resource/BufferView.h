#pragma once

#ifndef _BUFFERVIEW_H_
#define _BUFFERVIEW_H_

#include<Gear/Core/DX/Buffer.h>

#include<Gear/Core/DX/UploadHeap.h>

#include<Gear/Utils/Utils.h>

#include<Gear/Core/Resource/CounterBufferView.h>

#include"EngineResource.h"

//versatile buffer
class BufferView :public EngineResource
{
public:

	BufferView(Buffer* const buffer, const UINT structureByteStride, const DXGI_FORMAT format, const UINT size, const bool createSRV, const bool createUAV, const bool createVBV, const bool createIBV, const bool cpuWritable, const bool persistent);

	~BufferView();

	VertexBufferDesc getVertexBuffer() const;

	IndexBufferDesc getIndexBuffer() const;

	ShaderResourceDesc getSRVIndex() const;

	ShaderResourceDesc getUAVIndex() const;

	ClearUAVDesc getClearUAVDesc() const;

	CounterBufferView* getCounterBuffer() const;

	Buffer* getBuffer() const;

	void copyDescriptors() override;

	const bool hasSRV;

	const bool hasUAV;

private:

	friend class GraphicsContext;

	CounterBufferView* counterBuffer;

	UINT srvIndex;

	UINT uavIndex;

	D3D12_GPU_DESCRIPTOR_HANDLE viewGPUHandle;

	D3D12_CPU_DESCRIPTOR_HANDLE viewCPUHandle;

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
