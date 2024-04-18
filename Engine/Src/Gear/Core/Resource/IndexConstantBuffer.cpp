#include<Gear/Core/Resource/IndexConstantBuffer.h>

IndexConstantBuffer::IndexConstantBuffer(ConstantBuffer* const constantBuffer, const std::initializer_list<ShaderResourceDesc>& transitionDescs) :
	constantBuffer(constantBuffer)
{
	for (const ShaderResourceDesc& desc : transitionDescs)
	{
		descs.push_back(desc);
	}
}

IndexConstantBuffer::IndexConstantBuffer(ConstantBuffer* const constantBuffer):
	constantBuffer(constantBuffer)
{
}

void IndexConstantBuffer::setTransitionResources(const std::initializer_list<ShaderResourceDesc>& transitionDescs)
{
	descs.clear();

	for (const ShaderResourceDesc& desc : transitionDescs)
	{
		descs.push_back(desc);
	}

	std::vector<UINT> indices = std::vector<UINT>(transitionDescs.size());

	{
		UINT index = 0;

		for (const ShaderResourceDesc& desc : descs)
		{
			indices[index] = desc.resourceIndex;

			index++;
		}
	}

	constantBuffer->update(indices.data(), static_cast<UINT>(sizeof(UINT) * indices.size()));
}

IndexConstantBuffer::~IndexConstantBuffer()
{
	if (constantBuffer)
	{
		delete constantBuffer;
	}
}

D3D12_GPU_VIRTUAL_ADDRESS IndexConstantBuffer::getGPUAddress() const
{
	return constantBuffer->getGPUAddress();
}
