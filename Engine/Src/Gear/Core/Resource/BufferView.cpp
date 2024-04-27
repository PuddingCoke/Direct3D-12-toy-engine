#include<Gear/Core/Resource/BufferView.h>

BufferView::BufferView(Buffer* const buffer, const UINT structureByteStride, const DXGI_FORMAT format, const UINT size, const bool createSRV, const bool createUAV, const bool createVBV, const bool createIBV, const bool cpuWritable, const bool persistent) :
	buffer(buffer), srvIndex(0), uavIndex(0), vbv{}, ibv{}, uploadHeaps(nullptr), uploadHeapIndex(0), hasSRV(createSRV), hasUAV(createUAV)
{
	const UINT elementSize = (structureByteStride != 0 ? structureByteStride : Utils::getPixelSize(format));

	if (createSRV || createUAV)
	{
		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
		srvDesc.Buffer.FirstElement = 0;
		srvDesc.Buffer.NumElements = size / elementSize;
		srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;

		D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
		uavDesc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
		uavDesc.Buffer.FirstElement = 0;
		uavDesc.Buffer.NumElements = size / elementSize;
		uavDesc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_NONE;

		if (structureByteStride != 0)
		{
			srvDesc.Format = DXGI_FORMAT_UNKNOWN;
			srvDesc.Buffer.StructureByteStride = structureByteStride;

			uavDesc.Format = DXGI_FORMAT_UNKNOWN;
			uavDesc.Buffer.StructureByteStride = structureByteStride;
		}
		else if (format != DXGI_FORMAT_UNKNOWN)
		{
			srvDesc.Format = format;
			srvDesc.Buffer.StructureByteStride = 0;

			uavDesc.Format = format;
			uavDesc.Buffer.StructureByteStride = 0;
		}

		numSRVUAVCBVDescriptors = static_cast<UINT>(createSRV) + static_cast<UINT>(createUAV);

		if (numSRVUAVCBVDescriptors)
		{
			DescriptorHandle descriptorHandle;

			if (persistent)
			{
				descriptorHandle = GlobalDescriptorHeap::getResourceHeap()->allocStaticDescriptor(numSRVUAVCBVDescriptors);
			}
			else
			{
				descriptorHandle = GlobalDescriptorHeap::getNonShaderVisibleResourceHeap()->allocDynamicDescriptor(numSRVUAVCBVDescriptors);
			}

			srvUAVCBVHandleStart = descriptorHandle.getCPUHandle();

			if (createSRV)
			{
				GraphicsDevice::get()->CreateShaderResourceView(buffer->getResource(), &srvDesc, descriptorHandle.getCPUHandle());

				srvIndex = descriptorHandle.getCurrentIndex();

				descriptorHandle.move();
			}

			if (createUAV)
			{
				GraphicsDevice::get()->CreateUnorderedAccessView(buffer->getResource(), nullptr, &uavDesc, descriptorHandle.getCPUHandle());

				uavIndex = descriptorHandle.getCurrentIndex();

				descriptorHandle.move();
			}
		}
	}

	if (createVBV)
	{
		vbv.BufferLocation = buffer->getGPUAddress();
		vbv.SizeInBytes = size;
		vbv.StrideInBytes = elementSize;
	}

	if (createIBV)
	{
		ibv.BufferLocation = buffer->getGPUAddress();
		ibv.SizeInBytes = size;
		ibv.Format = format;
	}

	if (cpuWritable)
	{
		uploadHeaps = new UploadHeap * [Graphics::FrameBufferCount];

		for (UINT i = 0; i < Graphics::FrameBufferCount; i++)
		{
			uploadHeaps[i] = new UploadHeap(size);
		}
	}
}

BufferView::~BufferView()
{
	if (buffer)
	{
		delete buffer;
	}

	if (uploadHeaps)
	{
		for (UINT i = 0; i < Graphics::FrameBufferCount; i++)
		{
			delete uploadHeaps[i];
		}

		delete[] uploadHeaps;
	}
}

VertexBufferDesc BufferView::getVertexBuffer()
{
	VertexBufferDesc desc = {};
	desc.buffer = buffer;
	desc.vbv = vbv;

	return desc;
}

IndexBufferDesc BufferView::getIndexBuffer()
{
	IndexBufferDesc desc = {};
	desc.buffer = buffer;
	desc.ibv = ibv;

	return desc;
}


ShaderResourceDesc BufferView::getSRVIndex()
{
	ShaderResourceDesc desc = {};
	desc.type = ShaderResourceDesc::BUFFER;
	desc.state = ShaderResourceDesc::SRV;
	desc.resourceIndex = srvIndex;
	desc.bufferDesc.buffer = buffer;

	return desc;
}

ShaderResourceDesc BufferView::getUAVIndex()
{
	ShaderResourceDesc desc = {};
	desc.type = ShaderResourceDesc::BUFFER;
	desc.state = ShaderResourceDesc::UAV;
	desc.resourceIndex = uavIndex;
	desc.bufferDesc.buffer = buffer;

	return desc;
}

void BufferView::copyDescriptors()
{
	DescriptorHandle descriptorHandle = GlobalDescriptorHeap::getResourceHeap()->allocDynamicDescriptor(numSRVUAVCBVDescriptors);

	GraphicsDevice::get()->CopyDescriptorsSimple(numSRVUAVCBVDescriptors, descriptorHandle.getCPUHandle(), srvUAVCBVHandleStart, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	if (hasSRV)
	{
		srvIndex = descriptorHandle.getCurrentIndex();

		descriptorHandle.move();
	}

	if (hasUAV)
	{
		uavIndex = descriptorHandle.getCurrentIndex();

		descriptorHandle.move();
	}
}
