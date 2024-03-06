#include<Gear/Core/Resource/IndexConstantBuffer.h>

IndexConstantBuffer::IndexConstantBuffer(const std::initializer_list<ShaderResourceDesc>& transitionDescs, const bool cpuWritable, ID3D12GraphicsCommandList6* commandList, std::vector<Resource*>* transientResourcePool)
{
	for (const ShaderResourceDesc& desc : transitionDescs)
	{
		descs.push_back(desc);
	}

	const UINT indicesNum = ((descs.size() + 63) & ~63);

	indices = std::vector<UINT>(indicesNum);

	for (UINT i = 0; i < descs.size(); i++)
	{
		if (descs[i].type == ShaderResourceDesc::BUFFER)
		{
			indices[i] = descs[i].bufferDesc.resourceIndex;
		}
		else
		{
			indices[i] = descs[i].textureDesc.resourceIndex;
		}
	}

	constantBuffer = new ConstantBuffer(sizeof(UINT) * indices.size(), cpuWritable, indices.data(), commandList, transientResourcePool);
}

IndexConstantBuffer::IndexConstantBuffer(const UINT indicesNum)
{
	const UINT alignedIndicesNum = ((indicesNum + 63) & ~63);

	indices = std::vector<UINT>(alignedIndicesNum);

	constantBuffer = new ConstantBuffer(sizeof(UINT) * alignedIndicesNum, true, nullptr, nullptr, nullptr);
}

void IndexConstantBuffer::setTransitionResources(const std::initializer_list<ShaderResourceDesc>& transitionDescs)
{
	descs.clear();

	for (const ShaderResourceDesc& desc : transitionDescs)
	{
		descs.push_back(desc);
	}

	for (UINT i = 0; i < descs.size(); i++)
	{
		if (descs[i].type == ShaderResourceDesc::BUFFER)
		{
			indices[i] = descs[i].bufferDesc.resourceIndex;
		}
		else
		{
			indices[i] = descs[i].textureDesc.resourceIndex;
		}
	}

	constantBuffer->update(indices.data(), sizeof(UINT) * indices.size());
}

IndexConstantBuffer::~IndexConstantBuffer()
{
	delete constantBuffer;
}

D3D12_GPU_VIRTUAL_ADDRESS IndexConstantBuffer::getGPUAddress() const
{
	return constantBuffer->getGPUAddress();
}
