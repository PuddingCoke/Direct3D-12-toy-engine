#include<Gear/Core/Raytracing/TopLevelAS.h>

TopLevelAS::TopLevelAS(const bool allowUpdate) :
	scratchBufferSize(0), tlasBufferSize(0), instanceBufferSize(0), scratchBuffer(nullptr), tlasBuffer(nullptr), instanceBuffer(nullptr),
	buildFlag(allowUpdate ? D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_ALLOW_UPDATE
		: D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_NONE)

{
}

TopLevelAS::~TopLevelAS()
{
	if (scratchBuffer)
	{
		delete scratchBuffer;
	}

	if (tlasBuffer)
	{
		delete tlasBuffer;
	}

	if (instanceBuffer)
	{
		delete instanceBuffer;
	}
}

void TopLevelAS::addGeometryInstance(const GeometryInstance instance)
{
	instances.push_back(instance);
}

void TopLevelAS::generateTLAS(CommandList* const commandList)
{
	{
		D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS inputs = {};
		inputs.Type = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL;
		inputs.DescsLayout = D3D12_ELEMENTS_LAYOUT_ARRAY;
		inputs.NumDescs = instances.size();
		inputs.Flags = buildFlag;

		D3D12_RAYTRACING_ACCELERATION_STRUCTURE_PREBUILD_INFO info = {};

		GraphicsDevice::get()->GetRaytracingAccelerationStructurePrebuildInfo(&inputs, &info);

		scratchBufferSize = ROUND_UP(info.ScratchDataSizeInBytes, D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT);

		tlasBufferSize = ROUND_UP(info.ResultDataMaxSizeInBytes, D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT);

		instanceBufferSize = ROUND_UP(sizeof(D3D12_RAYTRACING_INSTANCE_DESC) * instances.size(), D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT);
	}

	scratchBuffer = new Buffer(scratchBufferSize, false, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

	tlasBuffer = new Buffer(tlasBufferSize, false, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE);

	instanceBuffer = new UploadHeap(instanceBufferSize);

	{
		std::vector<D3D12_RAYTRACING_INSTANCE_DESC> descs = std::vector<D3D12_RAYTRACING_INSTANCE_DESC>(instances.size());

		for (UINT i = 0; i < descs.size(); i++)
		{
			descs[i].InstanceID = instances[i].instanceID;

			descs[i].InstanceContributionToHitGroupIndex = instances[i].hitGroupIndex;

			descs[i].Flags = D3D12_RAYTRACING_INSTANCE_FLAG_NONE;

			memcpy(descs[i].Transform, &instances[i].transform, sizeof(descs[i].Transform));

			descs[i].AccelerationStructure = instances[i].blas->getBLASBuffer()->getGPUAddress();

			descs[i].InstanceMask = 0xFF;
		}

		instanceBuffer->update(descs.data(), sizeof(D3D12_RAYTRACING_INSTANCE_DESC) * descs.size());
	}

	D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC desc = {};
	desc.Inputs.Type = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL;
	desc.Inputs.DescsLayout = D3D12_ELEMENTS_LAYOUT_ARRAY;
	desc.Inputs.InstanceDescs = instanceBuffer->getGPUAddress();
	desc.Inputs.NumDescs = instances.size();
	desc.Inputs.Flags = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_NONE;
	desc.ScratchAccelerationStructureData = scratchBuffer->getGPUAddress();
	desc.DestAccelerationStructureData = tlasBuffer->getGPUAddress();
	//desc.SourceAccelerationStructureData = ;

	commandList->get()->BuildRaytracingAccelerationStructure(&desc, 0, nullptr);

	D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::UAV(tlasBuffer->getResource());

	commandList->resourceBarrier(1, &barrier);
}
