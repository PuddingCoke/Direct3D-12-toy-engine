#pragma once

#ifndef _COUNTERBUFFERVIEW_H_
#define _COUNTERBUFFERVIEW_H_

#include"D3D12Resource/Buffer.h"

#include"EngineResource.h"

class CounterBufferView :public EngineResource
{
public:

	CounterBufferView() = delete;

	CounterBufferView(const bool persistent);

	~CounterBufferView();

	ShaderResourceDesc getSRVIndex() const;

	ShaderResourceDesc getUAVIndex() const;

	ClearUAVDesc getClearUAVDesc() const;

	void copyDescriptors() override;

	Buffer* getBuffer() const;

private:

	uint32_t srvIndex;

	uint32_t uavIndex;

	D3D12_GPU_DESCRIPTOR_HANDLE viewGPUHandle;

	D3D12_CPU_DESCRIPTOR_HANDLE viewCPUHandle;

	Buffer* buffer;
};

#endif // !_COUNTERBUFFERVIEW_H_