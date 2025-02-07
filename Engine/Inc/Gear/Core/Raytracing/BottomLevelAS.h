#pragma once

#ifndef _BOTTOMLEVELAS_H_
#define _BOTTOMLEVELAS_H_

#include<Gear/Core/CommandList.h>

struct GeometryObject
{
	Buffer* vertexBuffer;//must not be nullptr

	uint64_t vertexByteOffset;//might create BLAS from single vertex buffer

	uint32_t vertexCount;//must not be 0

	uint32_t vertexSize;//must not be 0

	Buffer* indexBuffer;//optional

	uint64_t indexByteOffset;//optional

	uint32_t indexCount;//optional

	Buffer* transformBuffer;//optional

	uint64_t transformByteOffset;//optional

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
	std::vector<Buffer*> transitionBuffers;

	uint32_t scratchBufferSize;

	uint32_t blasBufferSize;

	const D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAGS buildFlag;

	Buffer* scratchBuffer;

	Buffer* blasBuffer;

};

#endif // !_BOTTOMLEVELAS_H_