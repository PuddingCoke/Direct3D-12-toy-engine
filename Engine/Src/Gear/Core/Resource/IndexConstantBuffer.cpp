#include<Gear/Core/Resource/IndexConstantBuffer.h>

IndexConstantBuffer::IndexConstantBuffer(const std::initializer_list<ShaderResourceDesc>& transitionDescs, const bool cpuWritable, ID3D12GraphicsCommandList6* commandList, std::vector<Resource*>* transientResourcePool)
{
	for (const ShaderResourceDesc& desc : transitionDescs)
	{
		descs.push_back(desc);
	}

	for (UINT i = 0; i < descs.size(); i++)
	{
		if (descs[i].type == ShaderResourceDesc::BUFFER)
		{
			indices.push_back(descs[i].bufferDesc.resourceIndex);
		}
		else
		{
			indices.push_back(descs[i].textureDesc.resourceIndex);
		}
	}

	constantBuffer = new ConstantBuffer(sizeof(UINT) * indices.size(), cpuWritable, indices.data(), commandList, transientResourcePool);
}

IndexConstantBuffer::IndexConstantBuffer(const UINT indicesNum)
{
	constantBuffer = new ConstantBuffer(sizeof(UINT) * indicesNum, true, nullptr, nullptr, nullptr);
}

void IndexConstantBuffer::setTransitionResources(const std::initializer_list<ShaderResourceDesc>& transitionDescs)
{
	descs.clear();

	indices.clear();

	for (const ShaderResourceDesc& desc : transitionDescs)
	{
		descs.push_back(desc);
	}

	for (UINT i = 0; i < descs.size(); i++)
	{
		if (descs[i].type == ShaderResourceDesc::BUFFER)
		{
			indices.push_back(descs[i].bufferDesc.resourceIndex);
		}
		else
		{
			indices.push_back(descs[i].textureDesc.resourceIndex);
		}
	}

	constantBuffer->update(indices.data(), sizeof(UINT) * indices.size());
}

IndexConstantBuffer::~IndexConstantBuffer()
{
}

D3D12_GPU_VIRTUAL_ADDRESS IndexConstantBuffer::getGPUAddress() const
{
	return constantBuffer->getGPUAddress();
}
