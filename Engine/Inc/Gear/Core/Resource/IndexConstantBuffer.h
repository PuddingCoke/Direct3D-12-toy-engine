#pragma once

#ifndef _INDEXCONSTANTBUFFER_H_
#define _INDEXCONSTANTBUFFER_H_

#include<Gear/Core/Resource/ConstantBuffer.h>

//Buffer SRV/UAV/CBV
//Texture SRV/UAV

//IndexConstantBuffer* icb=new IndexConstantBuffer({srv->getSRV(),});

//specialized constant buffer for indexing resources in CBV/SRV/UAV heap
//texture mipSlice srvIndexInHeap
//buffer cbvIndexInHeap
class IndexConstantBuffer
{
public:

	IndexConstantBuffer(const std::initializer_list<ShaderResourceDesc>& transitionDescs, const bool cpuWritable, ID3D12GraphicsCommandList6* commandList, std::vector<Resource*>* transientResourcePool);

	IndexConstantBuffer(const UINT indicesNum);

	IndexConstantBuffer(const IndexConstantBuffer&) = delete;

	void operator=(const IndexConstantBuffer&) = delete;

	~IndexConstantBuffer();

	D3D12_GPU_VIRTUAL_ADDRESS getGPUAddress() const;

	void setTransitionResources(const std::initializer_list<ShaderResourceDesc>& transitionDescs);

private:

	friend class GraphicsContext;

	ConstantBuffer* constantBuffer;

	std::vector<UINT> indices;

	std::vector<ShaderResourceDesc> descs;

};

#endif // !_INDEXCONSTANTBUFFER_H_
