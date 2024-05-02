#include<Gear/Core/Resource/CounterBufferView.h>

CounterBufferView::CounterBufferView(const bool persistent) :
	buffer(new Buffer(4, true, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS)), srvIndex(0), uavIndex(0), viewGPUHandle(), viewCPUHandle()
{
	numSRVUAVCBVDescriptors = 2;

	DescriptorHandle descriptorHandle;

	if (persistent)
	{
		descriptorHandle = GlobalDescriptorHeap::getResourceHeap()->allocStaticDescriptor(2);
	}
	else
	{
		descriptorHandle = GlobalDescriptorHeap::getNonShaderVisibleResourceHeap()->allocDynamicDescriptor(2);
	}

	srvUAVCBVHandleStart = descriptorHandle.getCPUHandle();

	{
		D3D12_SHADER_RESOURCE_VIEW_DESC desc = {};
		desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		desc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
		desc.Format = DXGI_FORMAT_R32_TYPELESS;
		desc.Buffer.FirstElement = 0;
		desc.Buffer.NumElements = 1;
		desc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_RAW;

		GraphicsDevice::get()->CreateShaderResourceView(buffer->getResource(), &desc, descriptorHandle.getCPUHandle());

		srvIndex = descriptorHandle.getCurrentIndex();

		descriptorHandle.move();
	}

	{
		D3D12_UNORDERED_ACCESS_VIEW_DESC desc = {};
		desc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
		desc.Format = DXGI_FORMAT_R32_TYPELESS;
		desc.Buffer.FirstElement = 0;
		desc.Buffer.NumElements = 1;
		desc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_RAW;

		GraphicsDevice::get()->CreateUnorderedAccessView(buffer->getResource(), nullptr, &desc, descriptorHandle.getCPUHandle());

		uavIndex = descriptorHandle.getCurrentIndex();

		if (persistent)
		{
			viewGPUHandle = descriptorHandle.getGPUHandle();

			const DescriptorHandle nonShaderVisibleHandle = GlobalDescriptorHeap::getNonShaderVisibleResourceHeap()->allocStaticDescriptor(1);

			GraphicsDevice::get()->CreateUnorderedAccessView(buffer->getResource(), nullptr, &desc, nonShaderVisibleHandle.getCPUHandle());

			viewCPUHandle = nonShaderVisibleHandle.getCPUHandle();
		}
		else
		{
			viewCPUHandle = descriptorHandle.getCPUHandle();

			//get viewGPUHandle later
		}
	}
}

CounterBufferView::~CounterBufferView()
{
	if (buffer)
	{
		delete buffer;
	}
}

ShaderResourceDesc CounterBufferView::getSRVIndex() const
{
	ShaderResourceDesc desc = {};
	desc.type = ShaderResourceDesc::BUFFER;
	desc.state = ShaderResourceDesc::SRV;
	desc.resourceIndex = srvIndex;
	desc.bufferDesc.buffer = buffer;

	return desc;
}

ShaderResourceDesc CounterBufferView::getUAVIndex() const
{
	ShaderResourceDesc desc = {};
	desc.type = ShaderResourceDesc::BUFFER;
	desc.state = ShaderResourceDesc::UAV;
	desc.resourceIndex = uavIndex;
	desc.bufferDesc.buffer = buffer;

	return desc;
}

ClearUAVDesc CounterBufferView::getClearUAVDesc() const
{
	ClearUAVDesc desc = {};
	desc.type = ClearUAVDesc::BUFFER;
	desc.bufferDesc.buffer = buffer;
	desc.viewGPUHandle = viewGPUHandle;
	desc.viewCPUHandle = viewCPUHandle;

	return desc;
}

void CounterBufferView::copyDescriptors()
{
	DescriptorHandle shaderVisibleHandle = GlobalDescriptorHeap::getResourceHeap()->allocDynamicDescriptor(numSRVUAVCBVDescriptors);

	GraphicsDevice::get()->CopyDescriptorsSimple(numSRVUAVCBVDescriptors, shaderVisibleHandle.getCPUHandle(), srvUAVCBVHandleStart, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	srvIndex = shaderVisibleHandle.getCurrentIndex();

	shaderVisibleHandle.move();

	uavIndex = shaderVisibleHandle.getCurrentIndex();

	viewGPUHandle = shaderVisibleHandle.getGPUHandle();
}

Buffer* CounterBufferView::getBuffer() const
{
	return buffer;
}
