#include<Gear/Core/Raytracing/BottomLevelAS.h>

BottomLevelAS::BottomLevelAS(const bool allowUpdate) :
	scratchBufferSize(0), blasBufferSize(0), scratchBuffer(nullptr), blasBuffer(nullptr),
	buildFlag(allowUpdate ? D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_ALLOW_UPDATE
		: D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_NONE)
{
}

BottomLevelAS::~BottomLevelAS()
{
	if (scratchBuffer)
	{
		delete scratchBuffer;
	}

	if (blasBuffer)
	{
		delete blasBuffer;
	}
}

void BottomLevelAS::addGeometryObject(const GeometryObject object)
{
	if (!(object.vertexBuffer->getStateTracking()) && !bitFlagSubset(object.vertexBuffer->getState(), D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE))
	{
		throw "input vertex buffer does not need state tracking however its internal state does not have D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE";
	}

	if (object.indexBuffer && !(object.indexBuffer->getStateTracking()) && !bitFlagSubset(object.indexBuffer->getState(), D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE))
	{
		throw "input index buffer does not need state tracking however its internal state does not have D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE";
	}

	if (object.transformBuffer && !(object.transformBuffer->getStateTracking()) && !bitFlagSubset(object.transformBuffer->getState(), D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE))
	{
		throw "input transform buffer does not need state tracking however its internal state does not have D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE";
	}

	D3D12_RAYTRACING_GEOMETRY_DESC desc = {};
	desc.Type = D3D12_RAYTRACING_GEOMETRY_TYPE_TRIANGLES;
	desc.Triangles.VertexBuffer.StartAddress = object.vertexBuffer->getGPUAddress() + object.vertexByteOffset;
	desc.Triangles.VertexBuffer.StrideInBytes = object.vertexSize;
	desc.Triangles.VertexCount = object.vertexCount;
	desc.Triangles.VertexFormat = DXGI_FORMAT_R32G32B32_FLOAT;

	if (object.vertexBuffer->getStateTracking())
	{
		transitionBuffers.push_back(object.vertexBuffer);
	}

	if (object.indexBuffer)
	{
		desc.Triangles.IndexBuffer = object.indexBuffer->getGPUAddress() + object.indexByteOffset;
		desc.Triangles.IndexCount = object.indexCount;
		desc.Triangles.IndexFormat = DXGI_FORMAT_R32_UINT;

		if (object.indexBuffer->getStateTracking())
		{
			transitionBuffers.push_back(object.indexBuffer);
		}
	}

	if (object.transformBuffer)
	{
		desc.Triangles.Transform3x4 = object.transformBuffer->getGPUAddress() + object.transformByteOffset;

		if (object.transformBuffer->getStateTracking())
		{
			transitionBuffers.push_back(object.transformBuffer);
		}
	}

	if (object.opaque)
	{
		desc.Flags = D3D12_RAYTRACING_GEOMETRY_FLAG_OPAQUE;
	}

	geometryDescs.push_back(desc);
}

void BottomLevelAS::generateBLAS(CommandList* const commandList)
{
	{
		D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS inputs = {};
		inputs.Type = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL;
		inputs.DescsLayout = D3D12_ELEMENTS_LAYOUT_ARRAY;
		inputs.NumDescs = geometryDescs.size();
		inputs.pGeometryDescs = geometryDescs.data();
		inputs.Flags = buildFlag;

		D3D12_RAYTRACING_ACCELERATION_STRUCTURE_PREBUILD_INFO info = {};

		GraphicsDevice::get()->GetRaytracingAccelerationStructurePrebuildInfo(&inputs, &info);

		scratchBufferSize = ROUND_UP(info.ScratchDataSizeInBytes, D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT);

		blasBufferSize = ROUND_UP(info.ResultDataMaxSizeInBytes, D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT);
	}

	scratchBuffer = new Buffer(scratchBufferSize, false, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COMMON);

	blasBuffer = new Buffer(blasBufferSize, false, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE);

	D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC desc = {};
	desc.Inputs.Type = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL;
	desc.Inputs.DescsLayout = D3D12_ELEMENTS_LAYOUT_ARRAY;
	desc.Inputs.NumDescs = geometryDescs.size();
	desc.Inputs.pGeometryDescs = geometryDescs.data();
	desc.Inputs.Flags = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_NONE;
	desc.ScratchAccelerationStructureData = scratchBuffer->getGPUAddress();
	desc.DestAccelerationStructureData = blasBuffer->getGPUAddress();
	//desc.SourceAccelerationStructureData = ;

	for (Buffer* const buffer : transitionBuffers)
	{
		commandList->pushResourceTrackList(buffer);

		buffer->setState(D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
	}

	commandList->transitionResources();

	commandList->get()->BuildRaytracingAccelerationStructure(&desc, 0, nullptr);

	D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::UAV(blasBuffer->getResource());

	commandList->resourceBarrier(1, &barrier);
}

Buffer* BottomLevelAS::getBLASBuffer() const
{
	return blasBuffer;
}
