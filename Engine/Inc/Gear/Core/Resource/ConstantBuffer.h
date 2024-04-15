#pragma once

#ifndef _CONSTANTBUFFER_H_
#define _CONSTANTBUFFER_H_

#include<Gear/Core/ConstantBufferPool.h>

#include"EngineResource.h"

//free to just copy pointer
class ConstantBuffer :public EngineResource
{
public:

	//buffer can be nullptr or not nullptr
	//buffer is nullptr then it's a dynamic constant buffer
	ConstantBuffer(Buffer* const buffer, const UINT size, const bool persistent);

	ConstantBuffer(const ConstantBuffer&) = delete;

	void operator=(const ConstantBuffer&) = delete;

	void update(const void* const data, const UINT size) const;

	~ConstantBuffer();

	ShaderResourceDesc getBufferIndex() const;

	D3D12_GPU_VIRTUAL_ADDRESS getGPUAddress() const;

	Buffer* getBuffer() const; 

	void copyDescriptors() override;

private:

	friend class RenderEngine;

	D3D12_GPU_VIRTUAL_ADDRESS gpuAddress;

	UINT bufferIndex;

	UINT poolIndex;

	Buffer* buffer;

	static ConstantBufferPool* bufferPools[ConstantBufferPool::numConstantBufferPool];

};

#endif // !_CONSTANTBUFFER_H_