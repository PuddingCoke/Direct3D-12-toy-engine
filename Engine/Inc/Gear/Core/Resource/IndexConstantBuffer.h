#pragma once

#ifndef _INDEXCONSTANTBUFFER_H_
#define _INDEXCONSTANTBUFFER_H_

#include<Gear/Core/DX/Resource/Buffer.h>

//Buffer SRV/UAV/CBV
//Texture SRV/UAV

//IndexConstantBuffer* icb=new IndexConstantBuffer({srv->getSRV(),});

//specialized constant buffer for indexing resources in CBV/SRV/UAV heap
//stored in non shader visible heap
//texture mipSlice srvIndexInHeap
//buffer cbvIndexInHeap
class IndexConstantBuffer
{
public:

	IndexConstantBuffer(const std::initializer_list<ShaderResourceDesc>& transitionDescs, const bool cpuWritable, ID3D12GraphicsCommandList6* commandList, std::vector<Resource*>& transientResourcePool);

	IndexConstantBuffer(const IndexConstantBuffer&);

	~IndexConstantBuffer();

private:

	friend class RenderPass;

	Buffer* buffer;

	std::vector<UINT> indices;

	std::vector<ShaderResourceDesc> descs;

};

#endif // !_INDEXCONSTANTBUFFER_H_
