#include<Gear/Core/Resource/StructuredBuffer.h>

StructuredBuffer::StructuredBuffer(Buffer* const buffer, const UINT structureByteStride, const UINT size, const bool cpuWritable, const bool persistent) :
	buffer(buffer), uploadHeaps(nullptr), uploadHeapIndex(0)
{
	if (cpuWritable)
	{
		uploadHeaps = new UploadHeap * [Graphics::FrameBufferCount];

		for (UINT i = 0; i < Graphics::FrameBufferCount; i++)
		{
			uploadHeaps[i] = new UploadHeap(size);
		}
	}

	{
		D3D12_SHADER_RESOURCE_VIEW_DESC desc = {};
		desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		desc.Format = DXGI_FORMAT_UNKNOWN;
		desc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
		desc.Buffer.FirstElement = 0;
		desc.Buffer.NumElements = size / structureByteStride;
		desc.Buffer.StructureByteStride = structureByteStride;
		desc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;

		numSRVUAVCBVDescriptors = 1;

		DescriptorHandle handle = DescriptorHandle();

		if (persistent)
		{
			handle = GlobalDescriptorHeap::getResourceHeap()->allocStaticDescriptor(numSRVUAVCBVDescriptors);
		}
		else
		{
			handle = GlobalDescriptorHeap::getNonShaderVisibleResourceHeap()->allocDynamicDescriptor(numSRVUAVCBVDescriptors);
		}

		srvUAVCBVHandleStart = handle.getCPUHandle();

		GraphicsDevice::get()->CreateShaderResourceView(buffer->getResource(), &desc, handle.getCPUHandle());

		bufferIndex = handle.getCurrentIndex();
	}
}

StructuredBuffer::~StructuredBuffer()
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

ShaderResourceDesc StructuredBuffer::getBufferIndex()
{
	ShaderResourceDesc desc = {};
	desc.type = ShaderResourceDesc::BUFFER;
	desc.state = ShaderResourceDesc::SRV;
	desc.resourceIndex = bufferIndex;
	desc.bufferDesc.buffer = buffer;

	return desc;
}

Buffer* StructuredBuffer::getBuffer() const
{
	return buffer;
}

void StructuredBuffer::copyDescriptors()
{
	const DescriptorHandle handle = GlobalDescriptorHeap::getResourceHeap()->allocDynamicDescriptor(numSRVUAVCBVDescriptors);

	GraphicsDevice::get()->CopyDescriptorsSimple(numSRVUAVCBVDescriptors, handle.getCPUHandle(), srvUAVCBVHandleStart, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	bufferIndex = handle.getCurrentIndex();
}
