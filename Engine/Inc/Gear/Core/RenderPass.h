#pragma once

#ifndef _RENDERPASS_H_
#define _RENDERPASS_H_

#include<Gear/Core/Resource/IndexConstantBuffer.h>
#include<Gear/Core/Resource/TextureRenderTarget.h>
#include<Gear/Core/Resource/TextureDepthStencil.h>
#include<Gear/Core/Resource/ConstantBuffer.h>
#include<Gear/Core/Resource/VertexBuffer.h>
#include<Gear/Core/Resource/IndexBuffer.h>

#include<Gear/Core/GraphicsContext.h>

#include<Gear/Input/Mouse.h>
#include<Gear/Input/Keyboard.h>
#include<Gear/Utils/Math.h>
#include<Gear/Utils/Random.h>
#include<Gear/Utils/Utils.h>
#include<Gear/Utils/Timer.h>

#include<future>
#include<vector>
#include<unordered_set>

class RenderPass
{
public:

	std::future<void> getPassResult();

	RenderPass();

	virtual ~RenderPass();

protected:

	ConstantBuffer* CreateConstantBuffer(const UINT size, const bool cpuWritable, const void* const data);

	IndexBuffer* CreateIndexBuffer(const DXGI_FORMAT format, const UINT size, const bool cpuWritable, const void* const data);

	VertexBuffer* CreateVertexBuffer(const UINT perVertexSize, const UINT size, const bool cpuWritable, const void* const data);

	IndexConstantBuffer* CreateIndexConstantBuffer(const std::initializer_list<ShaderResourceDesc>& descs, const bool cpuWritable);

	IndexConstantBuffer* CreateIndexConstantBuffer(const UINT indicesNum);

	TextureDepthStencil* CreateTextureDepthStencil(const UINT width, const UINT height, const DXGI_FORMAT resFormat, const UINT arraySize, const UINT mipLevels, const bool isTextureCube, const bool persistent);

	TextureRenderTarget* CreateTextureRenderTarget(const UINT width, const UINT height, const DXGI_FORMAT resFormat, const UINT arraySize, const UINT mipLevels, const bool isTextureCube, const bool persistent,
		const DXGI_FORMAT srvFormat, const DXGI_FORMAT uavFormat, const DXGI_FORMAT rtvFormat);

	TextureRenderTarget* CreateTextureRenderTarget(const std::string filePath, const bool isTextureCube, const bool persistent,
		const DXGI_FORMAT srvFormat = DXGI_FORMAT_UNKNOWN, const DXGI_FORMAT uavFormat = DXGI_FORMAT_UNKNOWN, const DXGI_FORMAT rtvFormat = DXGI_FORMAT_UNKNOWN);

	void deferredRelease(ConstantBuffer* const cb);

	void deferredRelease(IndexBuffer* const ib);

	void deferredRelease(IndexConstantBuffer* const icb);

	void deferredRelease(TextureDepthStencil* const tds);

	void deferredRelease(TextureRenderTarget* const trt);

	void deferredRelease(VertexBuffer* const vb);

	void begin();

	void end() const;

	virtual void recordCommand() = 0;

	GraphicsContext* context;

private:

	friend class RenderEngine;

	std::vector<Resource*> deferredReleaseResources[Graphics::FrameBufferCount];

	std::vector<ConstantBuffer*> deferredReleaseConstantBuffer[Graphics::FrameBufferCount];

	std::vector<IndexBuffer*> deferredReleaseIndexBuffer[Graphics::FrameBufferCount];

	std::vector<IndexConstantBuffer*> deferredReleaseIndexConstantBuffer[Graphics::FrameBufferCount];

	std::vector<TextureDepthStencil*> deferredReleaseTextureDepthStencil[Graphics::FrameBufferCount];

	std::vector<TextureRenderTarget*> deferredReleaseTextureRenderTarget[Graphics::FrameBufferCount];

	std::vector<VertexBuffer*> deferredReleaseVertexBuffer[Graphics::FrameBufferCount];

	CommandList* transitionCMD;

	template<typename T>
	void releaseResources(std::vector<T>& resources);

};

template<typename T>
inline void RenderPass::releaseResources(std::vector<T>& resources)
{
	for (const auto res : resources)
	{
		delete res;
	}

	resources.clear();
}

#endif // !_RENDERPASS_H_
