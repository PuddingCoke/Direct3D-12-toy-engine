#include<Gear/Core/Raytracing/TopLevelAS.h>

TopLevelAS::TopLevelAS(const bool allowUpdate, const bool persistent) :
	EngineResource(persistent),
	scratchBufferSize(0), tlasBufferSize(0), instanceBufferSize(0), scratchBuffer(nullptr), tlasBuffer(nullptr), instanceBuffer(nullptr),
	buildFlag(allowUpdate ? D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_ALLOW_UPDATE
		: D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_NONE),
	persistent(persistent)
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
		std::vector<D3D12_RAYTRACING_INSTANCE_DESC> instanceDescs = std::vector<D3D12_RAYTRACING_INSTANCE_DESC>(instances.size());

		for (UINT i = 0; i < instanceDescs.size(); i++)
		{
			instanceDescs[i].InstanceID = instances[i].instanceID;

			instanceDescs[i].InstanceContributionToHitGroupIndex = instances[i].hitGroupIndex;

			instanceDescs[i].Flags = D3D12_RAYTRACING_INSTANCE_FLAG_NONE;

			memcpy(instanceDescs[i].Transform, &instances[i].transform, sizeof(instanceDescs[i].Transform));

			instanceDescs[i].AccelerationStructure = instances[i].blas->getBLASBuffer()->getGPUAddress();

			instanceDescs[i].InstanceMask = 0xFF;
		}

		instanceBuffer->update(instanceDescs.data(), sizeof(D3D12_RAYTRACING_INSTANCE_DESC) * instanceDescs.size());
	}

	{
		D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC desc = {};
		desc.Inputs.Type = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL;
		desc.Inputs.DescsLayout = D3D12_ELEMENTS_LAYOUT_ARRAY;
		desc.Inputs.NumDescs = instances.size();
		desc.Inputs.InstanceDescs = instanceBuffer->getGPUAddress();
		desc.Inputs.Flags = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_NONE;
		desc.ScratchAccelerationStructureData = scratchBuffer->getGPUAddress();
		desc.DestAccelerationStructureData = tlasBuffer->getGPUAddress();
		//desc.SourceAccelerationStructureData = ;

		commandList->get()->BuildRaytracingAccelerationStructure(&desc, 0, nullptr);

		D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::UAV(tlasBuffer->getResource());

		commandList->resourceBarrier(1, &barrier);
	}

	{
		D3D12_SHADER_RESOURCE_VIEW_DESC desc = {};
		desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		desc.Format = DXGI_FORMAT_UNKNOWN;
		desc.ViewDimension = D3D12_SRV_DIMENSION_RAYTRACING_ACCELERATION_STRUCTURE;
		desc.RaytracingAccelerationStructure.Location = tlasBuffer->getGPUAddress();

		DescriptorHandle descriptorHandle = GlobalDescriptorHeap::getResourceHeap()->allocStaticDescriptor(1);

		GraphicsDevice::get()->CreateShaderResourceView(nullptr, &desc, descriptorHandle.getCPUHandle());

		srvIndex = descriptorHandle.getCurrentIndex();
	}
}

Buffer* TopLevelAS::getTLASBuffer() const
{
	return tlasBuffer;
}

ShaderResourceDesc TopLevelAS::getSRVIndex()
{
	ShaderResourceDesc desc = {};
	desc.type = ShaderResourceDesc::BUFFER;
	desc.state = ShaderResourceDesc::SRV;
	desc.resourceIndex = srvIndex;
	desc.bufferDesc.buffer = tlasBuffer;

	return desc;
}
