#include<Gear/Core/Resource/BufferView.h>

#include<Gear/Core/Graphics.h>

Core::Resource::BufferView::BufferView(D3D12Resource::Buffer* const buffer, const uint32_t structureByteStride, const DXGI_FORMAT format, const uint64_t size, const bool createSRV, const bool createUAV, const bool createVBV, const bool createIBV, const bool cpuWritable, const bool persistent) :
	EngineResource(persistent), buffer(buffer), counterBuffer(nullptr), vbv{}, srvIndex(0), uavIndex(0), uploadHeaps(nullptr), hasSRV(createSRV), hasUAV(createUAV), viewCPUHandle(), viewGPUHandle()
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

		D3D12Core::DescriptorHandle descriptorHandle;

		if (persistent)
		{
			descriptorHandle = Core::GlobalDescriptorHeap::getResourceHeap()->allocStaticDescriptor(numSRVUAVCBVDescriptors);
		}
		else
		{
			descriptorHandle = Core::GlobalDescriptorHeap::getNonShaderVisibleResourceHeap()->allocDynamicDescriptor(numSRVUAVCBVDescriptors);
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
				desc.Buffer.NumElements = static_cast<uint32_t>(size) / Core::FMT::getByteSize(format);
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

			Core::GraphicsDevice::get()->CreateShaderResourceView(buffer->getResource(), &desc, descriptorHandle.getCPUHandle());

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
				desc.Buffer.NumElements = static_cast<uint32_t>(size) / Core::FMT::getByteSize(format);
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
				Core::GraphicsDevice::get()->CreateUnorderedAccessView(buffer->getResource(), counterBuffer->getBuffer()->getResource(), &desc, descriptorHandle.getCPUHandle());
			}
			else
			{
				Core::GraphicsDevice::get()->CreateUnorderedAccessView(buffer->getResource(), nullptr, &desc, descriptorHandle.getCPUHandle());
			}

			uavIndex = descriptorHandle.getCurrentIndex();

			//for persistent resource we need an extra descriptor in non shader visible heap to get a view cpu handle
			//for non persistent resource we already have a descriptor in non shader visible heap
			if (persistent)
			{
				viewGPUHandle = descriptorHandle.getGPUHandle();

				const D3D12Core::DescriptorHandle nonShaderVisibleHandle = Core::GlobalDescriptorHeap::getNonShaderVisibleResourceHeap()->allocStaticDescriptor(1);

				if (isStructuredBuffer)
				{
					Core::GraphicsDevice::get()->CreateUnorderedAccessView(buffer->getResource(), counterBuffer->getBuffer()->getResource(), &desc, nonShaderVisibleHandle.getCPUHandle());
				}
				else
				{
					Core::GraphicsDevice::get()->CreateUnorderedAccessView(buffer->getResource(), nullptr, &desc, nonShaderVisibleHandle.getCPUHandle());
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
		vbv.StrideInBytes = (isStructuredBuffer ? structureByteStride : Core::FMT::getByteSize(format));
	}

	if (createIBV)
	{
		ibv.BufferLocation = buffer->getGPUAddress();
		ibv.SizeInBytes = static_cast<uint32_t>(size);
		ibv.Format = format;
	}

	if (cpuWritable)
	{
		uploadHeaps = new D3D12Resource::UploadHeap * [Core::Graphics::getFrameBufferCount()];

		for (uint32_t i = 0; i < Core::Graphics::getFrameBufferCount(); i++)
		{
			uploadHeaps[i] = new D3D12Resource::UploadHeap(size);
		}
	}
}

Core::Resource::BufferView::~BufferView()
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
		for (uint32_t i = 0; i < Core::Graphics::getFrameBufferCount(); i++)
		{
			delete uploadHeaps[i];
		}

		delete[] uploadHeaps;
	}
}

Core::Resource::D3D12Resource::VertexBufferDesc Core::Resource::BufferView::getVertexBuffer() const
{
	D3D12Resource::VertexBufferDesc desc = {};
	desc.buffer = buffer;
	desc.vbv = vbv;

	return desc;
}

Core::Resource::D3D12Resource::IndexBufferDesc Core::Resource::BufferView::getIndexBuffer() const
{
	D3D12Resource::IndexBufferDesc desc = {};
	desc.buffer = buffer;
	desc.ibv = ibv;

	return desc;
}


Core::Resource::D3D12Resource::ShaderResourceDesc Core::Resource::BufferView::getSRVIndex() const
{
	D3D12Resource::ShaderResourceDesc desc = {};
	desc.type = D3D12Resource::ShaderResourceDesc::BUFFER;
	desc.state = D3D12Resource::ShaderResourceDesc::SRV;
	desc.resourceIndex = srvIndex;
	desc.bufferDesc.buffer = buffer;

	return desc;
}

Core::Resource::D3D12Resource::ShaderResourceDesc Core::Resource::BufferView::getUAVIndex() const
{
	D3D12Resource::ShaderResourceDesc desc = {};
	desc.type = D3D12Resource::ShaderResourceDesc::BUFFER;
	desc.state = D3D12Resource::ShaderResourceDesc::UAV;
	desc.resourceIndex = uavIndex;
	desc.bufferDesc.buffer = buffer;

	if (counterBuffer)
	{
		desc.bufferDesc.counterBuffer = counterBuffer->getBuffer();
	}

	return desc;
}

Core::Resource::D3D12Resource::ClearUAVDesc Core::Resource::BufferView::getClearUAVDesc() const
{
	D3D12Resource::ClearUAVDesc desc = {};
	desc.type = D3D12Resource::ClearUAVDesc::BUFFER;
	desc.bufferDesc.buffer = buffer;
	desc.viewGPUHandle = viewGPUHandle;
	desc.viewCPUHandle = viewCPUHandle;

	return desc;
}

Core::Resource::CounterBufferView* Core::Resource::BufferView::getCounterBuffer() const
{
	return counterBuffer;
}

Core::Resource::D3D12Resource::Buffer* Core::Resource::BufferView::getBuffer() const
{
	return buffer;
}

void Core::Resource::BufferView::copyDescriptors()
{
	D3D12Core::DescriptorHandle shaderVisibleHandle = getTransientDescriptorHandle();

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

Core::Resource::BufferView::UpdateStruct Core::Resource::BufferView::getUpdateStruct(const void* const data, const uint64_t size)
{
	uploadHeaps[Core::Graphics::getFrameIndex()]->update(data, size);

	const UpdateStruct updateStruct = { buffer,uploadHeaps[Core::Graphics::getFrameIndex()] };

	return updateStruct;
}
