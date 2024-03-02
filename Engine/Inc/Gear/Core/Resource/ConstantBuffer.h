#pragma once

#ifndef _CONSTANTBUFFER_H_
#define _CONSTANTBUFFER_H_

#include<Gear/Core/DX/Resource/Buffer.h>
#include<Gear/Core/ConstantBufferPool.h>

class ConstantBuffer
{
public:

	ConstantBuffer(const UINT size, const bool cpuWritable, const void* const data, ID3D12GraphicsCommandList6* commandList, std::vector<Resource*>* transientResourcePool);

	ConstantBuffer(const ConstantBuffer&);

	void update(const void* const data, const UINT size);

	~ConstantBuffer();

	ShaderResourceDesc getBufferIndex() const;

	D3D12_GPU_VIRTUAL_ADDRESS getGPUAddress() const;

	Buffer* getBuffer() const;

private:

	friend class RenderEngine;

	const UINT size;

	D3D12_GPU_VIRTUAL_ADDRESS gpuAddress;

	UINT bufferIndex;

	UINT poolIndex;

	Buffer* buffer;

	static ConstantBufferPool* bufferPools[3];

};

#endif // !_CONSTANTBUFFER_H_