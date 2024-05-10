#pragma once

#ifndef _TOPLEVELAS_H_
#define _TOPLEVELAS_H_

#include<DirectXMath.h>

#include<Gear/Core/Resource/EngineResource.h>

#include"BottomLevelAS.h"

struct GeometryInstance
{
	BottomLevelAS* blas;

	DirectX::XMMATRIX transform;//require row major matrix and directxmath use row major matrix

	UINT instanceID;

	UINT hitGroupIndex;
};

class TopLevelAS :public EngineResource
{
public:

	TopLevelAS(const bool allowUpdate, const bool persistent);

	~TopLevelAS();

	void addGeometryInstance(const GeometryInstance instance);

	void generateTLAS(CommandList* const commandList);

	Buffer* getTLASBuffer() const;

	ShaderResourceDesc getSRVIndex();

private:

	std::vector<GeometryInstance> instances;

	UINT scratchBufferSize;

	UINT tlasBufferSize;

	UINT instanceBufferSize;

	const D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAGS buildFlag;

	Buffer* scratchBuffer;

	Buffer* tlasBuffer;

	UploadHeap* instanceBuffer;

	UINT srvIndex;

	const bool persistent;

};

#endif // !_TOPLEVELAS_H_
