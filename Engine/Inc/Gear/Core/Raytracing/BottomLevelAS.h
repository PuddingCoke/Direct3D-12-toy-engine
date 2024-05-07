#pragma once

#ifndef _BOTTOMLEVELAS_H_
#define _BOTTOMLEVELAS_H_

#include<Gear/Core/CommandList.h>

struct GeometryObject
{
	Buffer* vertexBuffer;//must not be nullptr

	UINT64 vertexByteOffset;//might create BLAS from single vertex buffer

	UINT vertexCount;//must not be 0

	UINT vertexSize;//must not be 0

	Buffer* indexBuffer;//optional

	UINT64 indexByteOffset;//optional

	UINT indexCount;//optional

	Buffer* transformBuffer;//optional

	UINT64 transformByteOffset;//optional

	bool opaque;//must choose a value
};

class BottomLevelAS
{
public:

	BottomLevelAS(const bool allowUpdate);

	~BottomLevelAS();

	void addGeometryObject(const GeometryObject object);

	void generateBLAS(CommandList* const commandList);

	Buffer* getBLASBuffer() const;

private:

	std::vector<D3D12_RAYTRACING_GEOMETRY_DESC> geometryDescs;

	//to build a BLAS all input resources need to at least have D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE set.
	//so we need to transition all buffers to this state
	//note:addGeometryObject will throw error if input buffer does't need state tracking while without mentioned state set
	std::vector<Buffer*> transitionBuffers;

	UINT scratchBufferSize;

	UINT blasBufferSize;

	const D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAGS buildFlag;

	Buffer* scratchBuffer;

	Buffer* blasBuffer;

};

#endif // !_BOTTOMLEVELAS_H_
