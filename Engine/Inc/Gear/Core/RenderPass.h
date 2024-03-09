#pragma once

#ifndef _RENDERPASS_H_
#define _RENDERPASS_H_

#include<Gear/Core/CommandList.h>
#include<Gear/Core/Resource/IndexConstantBuffer.h>
#include<Gear/Core/Resource/TextureRenderTarget.h>
#include<Gear/Core/Resource/TextureDepthStencil.h>
#include<Gear/Core/Resource/ConstantBuffer.h>
#include<Gear/Core/Resource/VertexBuffer.h>
#include<Gear/Core/Resource/IndexBuffer.h>

#include<Gear/Core/GraphicsContext.h>

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

	TextureDepthStencil* CreateTextureDepthStencil(const UINT width, const UINT height, const DXGI_FORMAT resFormat, const UINT arraySize, const UINT mipLevels, const bool isTextureCube);

	TextureRenderTarget* CreateTextureRenderTarget(const UINT width, const UINT height, const DXGI_FORMAT resFormat, const UINT arraySize, const UINT mipLevels, const bool isTextureCube,
		const DXGI_FORMAT srvFormat, const DXGI_FORMAT uavFormat, const DXGI_FORMAT rtvFormat);

	TextureRenderTarget* CreateTextureRenderTarget(const std::string filePath, const bool isTextureCube,
		const DXGI_FORMAT srvFormat = DXGI_FORMAT_UNKNOWN, const DXGI_FORMAT uavFormat = DXGI_FORMAT_UNKNOWN, const DXGI_FORMAT rtvFormat = DXGI_FORMAT_UNKNOWN);

	void begin();

	void end() const;

	virtual void recordCommand() = 0;

	GraphicsContext* context;

private:

	friend class RenderEngine;

	std::vector<Resource*> transientResources[Graphics::FrameBufferCount];

	CommandList* transitionCMD;

};

#endif // !_RENDERPASS_H_
