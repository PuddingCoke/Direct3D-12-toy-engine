#include<Gear/Core/RenderPass.h>

void RenderPass::launchTask()
{
	task = std::async(std::launch::async, [this]
		{
			begin();

			recordCommand();

			end();
		});
}

RenderPass::RenderPass() :
	context(new GraphicsContext()),
	transitionCMD(new CommandList(D3D12_COMMAND_LIST_TYPE_DIRECT)),
	task(std::async(std::launch::async, [this] {}))
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

	for (UINT i = 0; i < Graphics::FrameBufferCount; i++)
	{
		releaseResources(deferredReleaseResources[i]);
		releaseResources(deferredReleaseConstantBuffer[i]);
		releaseResources(deferredReleaseIndexBuffer[i]);
		releaseResources(deferredReleaseIndexConstantBuffer[i]);
		releaseResources(deferredReleaseTextureDepthStencil[i]);
		releaseResources(deferredReleaseTextureRenderTarget[i]);
		releaseResources(deferredReleaseVertexBuffer[i]);
	}
}

ConstantBuffer* RenderPass::CreateConstantBuffer(const UINT size, const bool cpuWritable, const void* const data)
{
	return new ConstantBuffer(size, cpuWritable, data, context->getCommandList()->get(), &deferredReleaseResources[Graphics::getFrameIndex()]);
}

IndexBuffer* RenderPass::CreateIndexBuffer(const DXGI_FORMAT format, const UINT size, const bool cpuWritable, const void* const data)
{
	return new IndexBuffer(format, size, cpuWritable, data, context->getCommandList()->get(), &deferredReleaseResources[Graphics::getFrameIndex()]);
}

VertexBuffer* RenderPass::CreateVertexBuffer(const UINT perVertexSize, const UINT size, const bool cpuWritable, const void* const data)
{
	return new VertexBuffer(perVertexSize, size, cpuWritable, data, context->getCommandList()->get(), &deferredReleaseResources[Graphics::getFrameIndex()]);
}

IndexConstantBuffer* RenderPass::CreateIndexConstantBuffer(const std::initializer_list<ShaderResourceDesc>& descs, const bool cpuWritable)
{
	return new IndexConstantBuffer(descs, cpuWritable, context->getCommandList()->get(), &deferredReleaseResources[Graphics::getFrameIndex()]);
}

IndexConstantBuffer* RenderPass::CreateIndexConstantBuffer(const UINT indicesNum)
{
	return new IndexConstantBuffer(indicesNum);
}

TextureDepthStencil* RenderPass::CreateTextureDepthStencil(const UINT width, const UINT height, const DXGI_FORMAT resFormat, const UINT arraySize, const UINT mipLevels, const bool isTextureCube,const bool persistent)
{
	return new TextureDepthStencil(width, height, resFormat, arraySize, mipLevels, isTextureCube, persistent);
}

TextureRenderTarget* RenderPass::CreateTextureRenderTarget(const UINT width, const UINT height, const DXGI_FORMAT resFormat, const UINT arraySize, const UINT mipLevels, const bool isTextureCube, const bool persistent, const DXGI_FORMAT srvFormat, const DXGI_FORMAT uavFormat, const DXGI_FORMAT rtvFormat)
{
	return new TextureRenderTarget(width, height, resFormat, arraySize, mipLevels, isTextureCube, persistent, srvFormat, uavFormat, rtvFormat);
}

TextureRenderTarget* RenderPass::CreateTextureRenderTarget(const std::string filePath, const bool isTextureCube, const bool persistent, const DXGI_FORMAT srvFormat, const DXGI_FORMAT uavFormat, const DXGI_FORMAT rtvFormat)
{
	return new TextureRenderTarget(filePath, context->getCommandList()->get(), &deferredReleaseResources[Graphics::getFrameIndex()], isTextureCube, persistent, srvFormat, uavFormat, rtvFormat);
}

TextureRenderTarget* RenderPass::CreateTextureRenderTarget(const UINT width, const UINT height, const Texture::TextureType type, const bool persistent)
{
	return new TextureRenderTarget(width, height, type, context->getCommandList()->get(), &deferredReleaseResources[Graphics::getFrameIndex()], persistent);
}

void RenderPass::deferredRelease(ConstantBuffer* const cb)
{
	deferredReleaseConstantBuffer[Graphics::getFrameIndex()].push_back(cb);
}

void RenderPass::deferredRelease(IndexBuffer* const ib)
{
	deferredReleaseIndexBuffer[Graphics::getFrameIndex()].push_back(ib);
}

void RenderPass::deferredRelease(IndexConstantBuffer* const icb)
{
	deferredReleaseIndexConstantBuffer[Graphics::getFrameIndex()].push_back(icb);
}

void RenderPass::deferredRelease(TextureDepthStencil* const tds)
{
	deferredReleaseTextureDepthStencil[Graphics::getFrameIndex()].push_back(tds);
}

void RenderPass::deferredRelease(TextureRenderTarget* const trt)
{
	deferredReleaseTextureRenderTarget[Graphics::getFrameIndex()].push_back(trt);
}

void RenderPass::deferredRelease(VertexBuffer* const vb)
{
	deferredReleaseVertexBuffer[Graphics::getFrameIndex()].push_back(vb);
}

void RenderPass::begin()
{
	releaseResources(deferredReleaseResources[Graphics::getFrameIndex()]);
	releaseResources(deferredReleaseConstantBuffer[Graphics::getFrameIndex()]);
	releaseResources(deferredReleaseIndexBuffer[Graphics::getFrameIndex()]);
	releaseResources(deferredReleaseIndexConstantBuffer[Graphics::getFrameIndex()]);
	releaseResources(deferredReleaseTextureDepthStencil[Graphics::getFrameIndex()]);
	releaseResources(deferredReleaseTextureRenderTarget[Graphics::getFrameIndex()]);
	releaseResources(deferredReleaseVertexBuffer[Graphics::getFrameIndex()]);

	context->begin();
}

void RenderPass::end() const
{
	context->end();
}
