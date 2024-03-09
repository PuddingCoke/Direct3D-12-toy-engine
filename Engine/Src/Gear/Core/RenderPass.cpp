#include<Gear/Core/RenderPass.h>

std::future<void> RenderPass::getPassResult()
{
	return std::async(std::launch::async, [&]
		{
			begin();

			recordCommand();

			end();
		});
}

RenderPass::RenderPass() :
	context(new GraphicsContext()),
	transitionCMD(new CommandList(D3D12_COMMAND_LIST_TYPE_DIRECT))
{
}

RenderPass::~RenderPass()
{
	if (context)
	{
		delete context;
	}

	if (transitionCMD)
	{
		delete transitionCMD;
	}

	for (std::vector<Resource*>& transientPool : transientResources)
	{
		if (transientPool.size())
		{
			for (Resource* res : transientPool)
			{
				delete res;
			}
		}
	}
}

ConstantBuffer* RenderPass::CreateConstantBuffer(const UINT size, const bool cpuWritable, const void* const data)
{
	return new ConstantBuffer(size, cpuWritable, data, context->getCommandList()->get(), &transientResources[Graphics::getFrameIndex()]);
}

IndexBuffer* RenderPass::CreateIndexBuffer(const DXGI_FORMAT format, const UINT size, const bool cpuWritable, const void* const data)
{
	return new IndexBuffer(format, size, cpuWritable, data, context->getCommandList()->get(), &transientResources[Graphics::getFrameIndex()]);
}

VertexBuffer* RenderPass::CreateVertexBuffer(const UINT perVertexSize, const UINT size, const bool cpuWritable, const void* const data)
{
	return new VertexBuffer(perVertexSize, size, cpuWritable, data, context->getCommandList()->get(), &transientResources[Graphics::getFrameIndex()]);
}

IndexConstantBuffer* RenderPass::CreateIndexConstantBuffer(const std::initializer_list<ShaderResourceDesc>& descs, const bool cpuWritable)
{
	return new IndexConstantBuffer(descs, cpuWritable, context->getCommandList()->get(), &transientResources[Graphics::getFrameIndex()]);
}

IndexConstantBuffer* RenderPass::CreateIndexConstantBuffer(const UINT indicesNum)
{
	return new IndexConstantBuffer(indicesNum);
}

TextureDepthStencil* RenderPass::CreateTextureDepthStencil(const UINT width, const UINT height, const DXGI_FORMAT resFormat, const UINT arraySize, const UINT mipLevels, const bool isTextureCube)
{
	return new TextureDepthStencil(width, height, resFormat, arraySize, mipLevels, isTextureCube);
}

TextureRenderTarget* RenderPass::CreateTextureRenderTarget(const UINT width, const UINT height, const DXGI_FORMAT resFormat, const UINT arraySize, const UINT mipLevels, const bool isTextureCube, const DXGI_FORMAT srvFormat, const DXGI_FORMAT uavFormat, const DXGI_FORMAT rtvFormat)
{
	return new TextureRenderTarget(width, height, resFormat, arraySize, mipLevels, isTextureCube, srvFormat, uavFormat, rtvFormat);
}

TextureRenderTarget* RenderPass::CreateTextureRenderTarget(const std::string filePath, const bool isTextureCube, const DXGI_FORMAT srvFormat, const DXGI_FORMAT uavFormat, const DXGI_FORMAT rtvFormat)
{
	return new TextureRenderTarget(filePath, context->getCommandList()->get(), &transientResources[Graphics::getFrameIndex()], isTextureCube, srvFormat, uavFormat, rtvFormat);
}

void RenderPass::begin()
{
	for (Resource* res : transientResources[Graphics::getFrameIndex()])
	{
		delete res;
	}

	transientResources[Graphics::getFrameIndex()].clear();

	context->begin();
}

void RenderPass::end()
{
	context->end();
}
