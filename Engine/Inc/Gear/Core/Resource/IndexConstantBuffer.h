#pragma once

#ifndef _INDEXCONSTANTBUFFER_H_
#define _INDEXCONSTANTBUFFER_H_

#include<Gear/Core/Resource/ConstantBuffer.h>

#include"EngineResource.h"

//Buffer SRV/UAV/CBV
//Texture SRV/UAV

//IndexConstantBuffer* icb=new IndexConstantBuffer({srv->getSRV(),});

//specialized constant buffer for indexing resources in CBV/SRV/UAV heap
//texture mipSlice srvIndexInHeap
//buffer cbvIndexInHeap
class IndexConstantBuffer :public EngineResource
{
public:

	IndexConstantBuffer(ConstantBuffer* const constantBuffer, const std::initializer_list<ShaderResourceDesc>& transitionDescs);

	IndexConstantBuffer(ConstantBuffer* const constantBuffer);

	IndexConstantBuffer(const IndexConstantBuffer&) = delete;

	void operator=(const IndexConstantBuffer&) = delete;

	~IndexConstantBuffer();

	D3D12_GPU_VIRTUAL_ADDRESS getGPUAddress() const;

	void setTransitionResources(const std::initializer_list<ShaderResourceDesc>& transitionDescs);

	const std::vector<ShaderResourceDesc>& getDescs() const;

	ConstantBuffer* getConstantBuffer() const;

private:

	std::vector<ShaderResourceDesc> descs;

	ConstantBuffer* constantBuffer;
};

#endif // !_INDEXCONSTANTBUFFER_H_