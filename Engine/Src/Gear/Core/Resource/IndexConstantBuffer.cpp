#include<Gear/Core/Resource/IndexConstantBuffer.h>

IndexConstantBuffer::IndexConstantBuffer(const std::initializer_list<TransitionDesc>& transitionDescs, const bool cpuWritable, ID3D12GraphicsCommandList6* commandList, std::vector<Resource*>& transientResourcePool)
{
	for (const TransitionDesc& desc : transitionDescs)
	{
		descs.push_back(desc);
	}

	for (UINT i = 0; i < descs.size(); i++)
	{
		if (descs[i].type == TransitionDesc::BUFFER)
		{
			indices.push_back(descs[i].buffer.resourceIndex);
		}
		else
		{
			indices.push_back(descs[i].texture.resourceIndex);
		}
	}

	buffer = new Buffer(sizeof(UINT) * indices.size(), false, cpuWritable, indices.data(), commandList, transientResourcePool);
}

IndexConstantBuffer::IndexConstantBuffer(const IndexConstantBuffer& icb) :
	buffer(new Buffer(*(icb.buffer))),
	descs(icb.descs),
	indices(icb.indices)
{
}

IndexConstantBuffer::~IndexConstantBuffer()
{
	if (buffer)
	{
		delete buffer;
	}
}
