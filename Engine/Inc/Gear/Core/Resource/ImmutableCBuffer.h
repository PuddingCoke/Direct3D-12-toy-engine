#pragma once

#ifndef _IMMUTABLECBUFFER_H_
#define _IMMUTABLECBUFFER_H_

#include"D3D12Resource/Buffer.h"

#include"EngineResource.h"

class ImmutableCBuffer :public EngineResource
{
public:

	ImmutableCBuffer() = delete;

	ImmutableCBuffer(const ImmutableCBuffer&) = delete;

	void operator=(const ImmutableCBuffer&) = delete;

	ImmutableCBuffer(Buffer* const buffer, const uint32_t size, const bool persistent);

	~ImmutableCBuffer();

	ShaderResourceDesc getBufferIndex() const;

	D3D12_GPU_VIRTUAL_ADDRESS getGPUAddress() const;

	Buffer* getBuffer() const;

	void copyDescriptors() override;

protected:

	D3D12_GPU_VIRTUAL_ADDRESS gpuAddress;

	uint32_t bufferIndex;

	Buffer* buffer;

};

#endif // !_IMMUTABLECBUFFER_H_