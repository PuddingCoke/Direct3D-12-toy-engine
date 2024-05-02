#pragma once

#ifndef _COUNTERBUFFERVIEW_H_
#define _COUNTERBUFFERVIEW_H_

#include<Gear/Core/DX/Buffer.h>

#include"EngineResource.h"

class CounterBufferView :public EngineResource
{
public:

	CounterBufferView(const bool persistent);

	~CounterBufferView();

	ShaderResourceDesc getSRVIndex() const;

	ShaderResourceDesc getUAVIndex() const;

	ClearUAVDesc getClearUAVDesc() const;

	void copyDescriptors() override;

	Buffer* getBuffer() const;

private:

	UINT srvIndex;

	UINT uavIndex;

	D3D12_GPU_DESCRIPTOR_HANDLE viewGPUHandle;

	D3D12_CPU_DESCRIPTOR_HANDLE viewCPUHandle;

	Buffer* buffer;
};

#endif // !_COUNTERBUFFERVIEW_H_

