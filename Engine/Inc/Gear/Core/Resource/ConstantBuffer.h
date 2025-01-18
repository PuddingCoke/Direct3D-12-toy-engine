#pragma once

#ifndef _CONSTANTBUFFER_H_
#define _CONSTANTBUFFER_H_

#include<Gear/Core/ConstantBufferManager.h>

#include"EngineResource.h"

//feel free to copy pointer because an effecient management strategy we use here
//but be cautious here you should release transient constantbuffer by using resource manager(resManager)
//otherwise there will be data overwritten
class ConstantBuffer :public EngineResource
{
public:

	ConstantBuffer(Buffer* const buffer, const uint32_t size, const bool persistent);

	ConstantBuffer(const ConstantBuffer&) = delete;

	void operator=(const ConstantBuffer&) = delete;

	void update(const void* const data, const uint32_t size) const;

	~ConstantBuffer();

	ShaderResourceDesc getBufferIndex() const;

	D3D12_GPU_VIRTUAL_ADDRESS getGPUAddress() const;

	Buffer* getBuffer() const;

	void copyDescriptors() override;

private:

	D3D12_GPU_VIRTUAL_ADDRESS gpuAddress;

	uint32_t bufferIndex;

	uint32_t regionIndex;

	Buffer* buffer;

};

#endif // !_CONSTANTBUFFER_H_