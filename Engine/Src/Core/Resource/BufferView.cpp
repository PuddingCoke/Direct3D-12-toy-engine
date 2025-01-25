#include<Gear/Core/Resource/BufferView.h>

BufferView::BufferView(Buffer* const buffer, const uint32_t structureByteStride, const DXGI_FORMAT format, const uint64_t size, const bool createSRV, const bool createUAV, const bool createVBV, const bool createIBV, const bool cpuWritable, const bool persistent) :
	EngineResource(persistent), buffer(buffer), counterBuffer(nullptr), vbv{}, srvIndex(0), uavIndex(0), uploadHeaps(nullptr), uploadHeapIndex(0), hasSRV(createSRV), hasUAV(createUAV), viewCPUHandle(), viewGPUHandle()
{
	numSRVUAVCBVDescriptors = static_cast<uint32_t>(createSRV) + static_cast<uint32_t>(createUAV);

	const bool isTypedBuffer = (structureByteStride == 0 && format != DXGI_FORMAT_UNKNOWN);

	const bool isStructuredBuffer = (structureByteStride != 0 && format == DXGI_FORMAT_UNKNOWN);

	const bool isByteAddressBuffer = (structureByteStride == 0 && format == DXGI_FORMAT_UNKNOWN);

	if (numSRVUAVCBVDescriptors)
	{
		//create counter buffer for structured buffer
		if (isStructuredBuffer && createUAV)
		{
			counterBuffer = new CounterBufferView(persistent);
		}

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
			D3D12_SHADER_RESOURCE_VIEW_DESC desc = {};
			desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
			desc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;

			if (isTypedBuffer)
			{
				desc.Format = format;
				desc.Buffer.NumElements = static_cast<uint32_t>(size) / Utils::getPixelSize(format);
			}
			else if (isStructuredBuffer)
			{
				desc.Buffer.NumElements = static_cast<uint32_t>(size) / structureByteStride;
				desc.Buffer.StructureByteStride = structureByteStride;
			}
			else if (isByteAddressBuffer)
			{
				desc.Format = DXGI_FORMAT_R32_TYPELESS;
				desc.Buffer.NumElements = static_cast<uint32_t>(size) / 4;
				desc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_RAW;
			}

			GraphicsDevice::get()->CreateShaderResourceView(buffer->getResource(), &desc, descriptorHandle.getCPUHandle());

			srvIndex = descriptorHandle.getCurrentIndex();

			descriptorHandle.move();
		}

		if (createUAV)
		{
			D3D12_UNORDERED_ACCESS_VIEW_DESC desc = {};
			desc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;

			if (isTypedBuffer)
			{
				desc.Format = format;
				desc.Buffer.NumElements = static_cast<uint32_t>(size) / Utils::getPixelSize(format);
			}
			else if (isStructuredBuffer)
			{
				desc.Buffer.NumElements = static_cast<uint32_t>(size) / structureByteStride;
				desc.Buffer.StructureByteStride = structureByteStride;
			}
			else if (isByteAddressBuffer)
			{
				desc.Format = DXGI_FORMAT_R32_TYPELESS;
				desc.Buffer.NumElements = static_cast<uint32_t>(size) / 4;
				desc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_RAW;
			}

			if (isStructuredBuffer)
			{
				GraphicsDevice::get()->CreateUnorderedAccessView(buffer->getResource(), counterBuffer->getBuffer()->getResource(), &desc, descriptorHandle.getCPUHandle());
			}
			else
			{
				GraphicsDevice::get()->CreateUnorderedAccessView(buffer->getResource(), nullptr, &desc, descriptorHandle.getCPUHandle());
			}

			uavIndex = descriptorHandle.getCurrentIndex();

			//for persistent resource we need an extra descriptor in non shader visible heap to get a view cpu handle
			//for non persistent resource we already have a descriptor in non shader visible heap
			if (persistent)
			{
				viewGPUHandle = descriptorHandle.getGPUHandle();

				const DescriptorHandle nonShaderVisibleHandle = GlobalDescriptorHeap::getNonShaderVisibleResourceHeap()->allocStaticDescriptor(1);

				if (isStructuredBuffer)
				{
					GraphicsDevice::get()->CreateUnorderedAccessView(buffer->getResource(), counterBuffer->getBuffer()->getResource(), &desc, nonShaderVisibleHandle.getCPUHandle());
				}
				else
				{
					GraphicsDevice::get()->CreateUnorderedAccessView(buffer->getResource(), nullptr, &desc, nonShaderVisibleHandle.getCPUHandle());
				}

				viewCPUHandle = nonShaderVisibleHandle.getCPUHandle();
			}
			else
			{
				viewCPUHandle = descriptorHandle.getCPUHandle();

				//get viewGPUHandle later
			}
		}
	}

	if (createVBV)
	{
		vbv.BufferLocation = buffer->getGPUAddress();
		vbv.SizeInBytes = static_cast<uint32_t>(size);
		vbv.StrideInBytes = (isStructuredBuffer ? structureByteStride : Utils::getPixelSize(format));
	}

	if (createIBV)
	{
		ibv.BufferLocation = buffer->getGPUAddress();
		ibv.SizeInBytes = static_cast<uint32_t>(size);
		ibv.Format = format;
	}

	if (cpuWritable)
	{
		uploadHeaps = new UploadHeap * [Graphics::getFrameBufferCount()];

		for (uint32_t i = 0; i < Graphics::getFrameBufferCount(); i++)
		{
			uploadHeaps[i] = new UploadHeap(size);
		}
	}
}

BufferView::~BufferView()
{
	if (counterBuffer)
	{
		delete counterBuffer;
	}

	if (buffer)
	{
		delete buffer;
	}

	if (uploadHeaps)
	{
		for (uint32_t i = 0; i < Graphics::getFrameBufferCount(); i++)
		{
			delete uploadHeaps[i];
		}

		delete[] uploadHeaps;
	}
}

VertexBufferDesc BufferView::getVertexBuffer() const
{
	VertexBufferDesc desc = {};
	desc.buffer = buffer;
	desc.vbv = vbv;

	return desc;
}

IndexBufferDesc BufferView::getIndexBuffer() const
{
	IndexBufferDesc desc = {};
	desc.buffer = buffer;
	desc.ibv = ibv;

	return desc;
}


ShaderResourceDesc BufferView::getSRVIndex() const
{
	ShaderResourceDesc desc = {};
	desc.type = ShaderResourceDesc::BUFFER;
	desc.state = ShaderResourceDesc::SRV;
	desc.resourceIndex = srvIndex;
	desc.bufferDesc.buffer = buffer;

	return desc;
}

ShaderResourceDesc BufferView::getUAVIndex() const
{
	ShaderResourceDesc desc = {};
	desc.type = ShaderResourceDesc::BUFFER;
	desc.state = ShaderResourceDesc::UAV;
	desc.resourceIndex = uavIndex;
	desc.bufferDesc.buffer = buffer;

	if (counterBuffer)
	{
		desc.bufferDesc.counterBuffer = counterBuffer->getBuffer();
	}

	return desc;
}

ClearUAVDesc BufferView::getClearUAVDesc() const
{
	ClearUAVDesc desc = {};
	desc.type = ClearUAVDesc::BUFFER;
	desc.bufferDesc.buffer = buffer;
	desc.viewGPUHandle = viewGPUHandle;
	desc.viewCPUHandle = viewCPUHandle;

	return desc;
}

CounterBufferView* BufferView::getCounterBuffer() const
{
	return counterBuffer;
}

Buffer* BufferView::getBuffer() const
{
	return buffer;
}

void BufferView::copyDescriptors()
{
	DescriptorHandle shaderVisibleHandle = getTransientDescriptorHandle();

	if (hasSRV)
	{
		srvIndex = shaderVisibleHandle.getCurrentIndex();

		shaderVisibleHandle.move();
	}

	if (hasUAV)
	{
		uavIndex = shaderVisibleHandle.getCurrentIndex();

		viewGPUHandle = shaderVisibleHandle.getGPUHandle();
	}
}

BufferView::UpdateStruct BufferView::update(const void* const data, const uint64_t size)
{
	const UpdateStruct updateStruct = { buffer,uploadHeaps[uploadHeapIndex] };

	uploadHeaps[uploadHeapIndex]->update(data, size);

	uploadHeapIndex = (uploadHeapIndex + 1) % Graphics::getFrameBufferCount();

	return updateStruct;
}
