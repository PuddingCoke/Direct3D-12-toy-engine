#pragma once

#ifndef _CONSTANTBUFFER_H_
#define _CONSTANTBUFFER_H_

#include<Gear/Core/DX/Resource/Buffer.h>

class ConstantBuffer
{
public:

	ConstantBuffer(const UINT size, const bool stateTracking, const bool cpuWritable, const void* const data, ID3D12GraphicsCommandList6* commandList, std::vector<Resource*>& transientResourcePool);

	ConstantBuffer(const ConstantBuffer&);

	~ConstantBuffer();

	ShaderResourceDesc getBufferIndex() const;

	Buffer* getBuffer() const;

private:

	UINT bufferIndex;

	Buffer* buffer;

};

#endif // !_CONSTANTBUFFER_H_
