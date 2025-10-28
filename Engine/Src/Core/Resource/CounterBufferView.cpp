#include<Gear/Core/Resource/CounterBufferView.h>

Core::Resource::CounterBufferView::CounterBufferView(const bool persistent) :
	ResourceBase(persistent), buffer(new D3D12Resource::Buffer(4, true, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS)), srvIndex(0), uavIndex(0), viewGPUHandle(), viewCPUHandle()
{
	numSRVUAVCBVDescriptors = 2;

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

	{
		D3D12_SHADER_RESOURCE_VIEW_DESC desc = {};
		desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		desc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
		desc.Format = DXGI_FORMAT_R32_TYPELESS;
		desc.Buffer.FirstElement = 0;
		desc.Buffer.NumElements = 1;
		desc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_RAW;

		Core::GraphicsDevice::get()->CreateShaderResourceView(buffer->getResource(), &desc, descriptorHandle.getCPUHandle());

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

		Core::GraphicsDevice::get()->CreateUnorderedAccessView(buffer->getResource(), nullptr, &desc, descriptorHandle.getCPUHandle());

		uavIndex = descriptorHandle.getCurrentIndex();

		if (persistent)
		{
			viewGPUHandle = descriptorHandle.getGPUHandle();

			const D3D12Core::DescriptorHandle nonShaderVisibleHandle = Core::GlobalDescriptorHeap::getNonShaderVisibleResourceHeap()->allocStaticDescriptor(1);

			Core::GraphicsDevice::get()->CreateUnorderedAccessView(buffer->getResource(), nullptr, &desc, nonShaderVisibleHandle.getCPUHandle());

			viewCPUHandle = nonShaderVisibleHandle.getCPUHandle();
		}
		else
		{
			viewCPUHandle = descriptorHandle.getCPUHandle();

			//get viewGPUHandle later
		}
	}
}

Core::Resource::CounterBufferView::~CounterBufferView()
{
	if (buffer)
	{
		delete buffer;
	}
}

Core::Resource::D3D12Resource::ShaderResourceDesc Core::Resource::CounterBufferView::getSRVIndex() const
{
	D3D12Resource::ShaderResourceDesc desc = {};
	desc.type = D3D12Resource::ShaderResourceDesc::BUFFER;
	desc.state = D3D12Resource::ShaderResourceDesc::SRV;
	desc.resourceIndex = srvIndex;
	desc.bufferDesc.buffer = buffer;

	return desc;
}

Core::Resource::D3D12Resource::ShaderResourceDesc Core::Resource::CounterBufferView::getUAVIndex() const
{
	D3D12Resource::ShaderResourceDesc desc = {};
	desc.type = D3D12Resource::ShaderResourceDesc::BUFFER;
	desc.state = D3D12Resource::ShaderResourceDesc::UAV;
	desc.resourceIndex = uavIndex;
	desc.bufferDesc.buffer = buffer;

	return desc;
}

Core::Resource::D3D12Resource::ClearUAVDesc Core::Resource::CounterBufferView::getClearUAVDesc() const
{
	D3D12Resource::ClearUAVDesc desc = {};
	desc.type = D3D12Resource::ClearUAVDesc::BUFFER;
	desc.bufferDesc.buffer = buffer;
	desc.viewGPUHandle = viewGPUHandle;
	desc.viewCPUHandle = viewCPUHandle;

	return desc;
}

void Core::Resource::CounterBufferView::copyDescriptors()
{
	D3D12Core::DescriptorHandle shaderVisibleHandle = getTransientDescriptorHandle();

	srvIndex = shaderVisibleHandle.getCurrentIndex();

	shaderVisibleHandle.move();

	uavIndex = shaderVisibleHandle.getCurrentIndex();

	viewGPUHandle = shaderVisibleHandle.getGPUHandle();
}

Core::Resource::D3D12Resource::Buffer* Core::Resource::CounterBufferView::getBuffer() const
{
	return buffer;
}
