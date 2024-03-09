#pragma once

#ifndef _CONSTANTBUFFER_H_
#define _CONSTANTBUFFER_H_

#include<Gear/Core/DX/Resource/Buffer.h>
#include<Gear/Core/ConstantBufferPool.h>

//we use a very effective way to achieve state sync for constant buffer
//so feel free to just copy constant buffer pointer
class ConstantBuffer
{
public:

	ConstantBuffer(const UINT size, const bool cpuWritable, const void* const data, ID3D12GraphicsCommandList6* commandList, std::vector<Resource*>* transientResourcePool);

	ConstantBuffer(const ConstantBuffer&) = delete;

	void operator=(const ConstantBuffer&) = delete;

	void update(const void* const data, const UINT size) const;

	~ConstantBuffer();

	ShaderResourceDesc getBufferIndex() const;

	D3D12_GPU_VIRTUAL_ADDRESS getGPUAddress() const;

	Buffer* getBuffer() const;

private:

	friend class RenderEngine;

	D3D12_GPU_VIRTUAL_ADDRESS gpuAddress;

	UINT bufferIndex;

	UINT poolIndex;

	Buffer* buffer;

	static ConstantBufferPool* bufferPools[Graphics::FrameBufferCount];

};

#endif // !_CONSTANTBUFFER_H_