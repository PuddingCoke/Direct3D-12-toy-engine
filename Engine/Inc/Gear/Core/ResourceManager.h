#pragma once

#ifndef _RESOURCEMANAGER_H_
#define _RESOURCEMANAGER_H_

#include<Gear/Core/GraphicsContext.h>
#include<Gear/Core/PipelineState.h>
#include<Gear/Utils/Math.h>

#include<Gear/Core/DX/Resource/Resource.h>
#include<Gear/Core/DX/Resource/Buffer.h>
#include<Gear/Core/DX/Resource/Texture.h>
#include<Gear/Core/DX/Resource/UploadHeap.h>

#include<Gear/Core/Resource/EngineResource.h>
#include<Gear/Core/Resource/IndexConstantBuffer.h>
#include<Gear/Core/Resource/TextureRenderTarget.h>
#include<Gear/Core/Resource/TextureDepthStencil.h>
#include<Gear/Core/Resource/ConstantBuffer.h>
#include<Gear/Core/Resource/VertexBuffer.h>
#include<Gear/Core/Resource/IndexBuffer.h>
#include<Gear/Core/Resource/SwapTexture.h>

enum class RandomDataType
{
	NOISE,
	GAUSS
};

//a tool for creating both low level resource and high level resource
//some method is commandList dependent so you need a ResourceManager instance
class ResourceManager
{
public:

	ResourceManager(GraphicsContext* const context);

	~ResourceManager();

	void release(Resource* const resource);

	void release(EngineResource* const engineResource);

	void cleanTransientResources();

	//following methods create low level resources
	//stateTracking is true

	//create buffer from data
	Buffer* createBuffer(const void* const data, const UINT size, const D3D12_RESOURCE_FLAGS resFlags);

	//create texture from file
	Texture* createTexture(const std::string filePath, const D3D12_RESOURCE_FLAGS resFlags, bool* const isTextureCube);

	//create texture from random data
	Texture* createTexture(const UINT width, const UINT height, const RandomDataType type, const D3D12_RESOURCE_FLAGS resFlags);

	//following methods create high level resources

	ConstantBuffer* createConstantBuffer(const UINT size, const bool cpuWritable, const void* const data, const bool persistent);

	static ConstantBuffer* createConstantBuffer(const UINT size, const bool persistent);

	//data must not be nullptr
	IndexBuffer* createIndexBuffer(const DXGI_FORMAT format, const UINT size, const bool cpuWritable, const void* const data);

	static IndexBuffer* createIndexBuffer(const DXGI_FORMAT format, const UINT size);

	//data must not be nullptr
	VertexBuffer* createVertexBuffer(const UINT perVertexSize, const UINT size, const bool cpuWritable, const void* const data);

	static VertexBuffer* createVertexBuffer(const UINT perVertexSize, const UINT size);

	IndexConstantBuffer* createIndexConstantBuffer(const std::initializer_list<ShaderResourceDesc>& descs, const bool cpuWritable, const bool persistent);

	static IndexConstantBuffer* createIndexConstantBuffer(const UINT indicesNum, const bool persistent);

	static TextureDepthStencil* createTextureDepthStencil(const UINT width, const UINT height, const DXGI_FORMAT resFormat, const UINT arraySize, const UINT mipLevels, const bool isTextureCube, const bool persistent);

	//load jpg jpeg png hdr dds(auto detect texturecube) textures
	TextureRenderTarget* createTextureRenderTarget(const std::string filePath, const bool persistent,
		const DXGI_FORMAT srvFormat = DXGI_FORMAT_UNKNOWN, const DXGI_FORMAT uavFormat = DXGI_FORMAT_UNKNOWN, const DXGI_FORMAT rtvFormat = DXGI_FORMAT_UNKNOWN);

	//create texture render target from random data
	TextureRenderTarget* createTextureRenderTarget(const UINT width, const UINT height, const RandomDataType type, const bool persistent,
		const DXGI_FORMAT srvFormat = DXGI_FORMAT_UNKNOWN, const DXGI_FORMAT uavFormat = DXGI_FORMAT_UNKNOWN, const DXGI_FORMAT rtvFormat = DXGI_FORMAT_UNKNOWN);

	//create texture render target from custom parameters
	static TextureRenderTarget* createTextureRenderTarget(const UINT width, const UINT height, const DXGI_FORMAT resFormat, const UINT arraySize, const UINT mipLevels, const bool isTextureCube, const bool persistent,
		const DXGI_FORMAT srvFormat, const DXGI_FORMAT uavFormat, const DXGI_FORMAT rtvFormat);

	//create texture cube from equirectangular map
	TextureRenderTarget* createTextureCube(const std::string filePath, const UINT texturecubeResolution, const bool persistent,
		const DXGI_FORMAT srvFormat = DXGI_FORMAT_UNKNOWN, const DXGI_FORMAT uavFormat = DXGI_FORMAT_UNKNOWN, const DXGI_FORMAT rtvFormat = DXGI_FORMAT_UNKNOWN);

	//create texture cube from 6 seperate textures
	TextureRenderTarget* createTextureCube(const std::initializer_list<std::string> texturesPath,const bool persistent,
		const DXGI_FORMAT srvFormat = DXGI_FORMAT_UNKNOWN, const DXGI_FORMAT uavFormat = DXGI_FORMAT_UNKNOWN, const DXGI_FORMAT rtvFormat = DXGI_FORMAT_UNKNOWN);

private:

	std::vector<Resource*> resources[Graphics::FrameBufferCount];

	std::vector<EngineResource*> engineResources[Graphics::FrameBufferCount];

	//high level task such as create texture cube from equirectangular map
	GraphicsContext* const context;

	//low level task such as initalize texture with data
	CommandList* const commandList;

};

#endif // !_RESOURCEMANAGER_H_