#pragma once

#ifndef _RESOURCEMANAGER_H_
#define _RESOURCEMANAGER_H_

#include<DDSTextureLoader/DDSTextureLoader12.h>
#include<WICTextureLoader/WICTextureLoader12.h>
#include<DirectXTex/DirectXTex.h>

#include<Gear/Core/GraphicsContext.h>
#include<Gear/Core/PipelineState.h>

#include<Gear/Utils/Math.h>
#include<Gear/Utils/Random.h>

#include<Gear/Core/DX/Buffer.h>
#include<Gear/Core/DX/Texture.h>
#include<Gear/Core/DX/UploadHeap.h>

#include<Gear/Core/Resource/SwapTexture.h>
#include<Gear/Core/Resource/ConstantBuffer.h>
#include<Gear/Core/Resource/IndexConstantBuffer.h>
#include<Gear/Core/Resource/BufferView.h>
#include<Gear/Core/Resource/TextureRenderView.h>
#include<Gear/Core/Resource/TextureDepthView.h>

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

	GraphicsContext* getGraphicsContext() const;

	CommandList* getCommandList() const;

	//create low level resource

	//create buffer from data
	Buffer* createBuffer(const void* const data, const uint64_t size, const D3D12_RESOURCE_FLAGS resFlags);

	//create texture from file
	Texture* createTexture(const std::wstring& filePath, const D3D12_RESOURCE_FLAGS resFlags, bool* const isTextureCube);

	//create texture from random data
	Texture* createTexture(const uint32_t width, const uint32_t height, const RandomDataType type, const D3D12_RESOURCE_FLAGS resFlags);

	//create high level resource

	ConstantBuffer* createConstantBuffer(const uint32_t size, const bool cpuWritable, const void* const data, const bool persistent);

	static ConstantBuffer* createConstantBuffer(const uint32_t size, const bool persistent);

	BufferView* createTypedBufferView(const DXGI_FORMAT format, const uint64_t size, const bool createSRV, const bool createUAV, const bool createVBV, const bool createIBV, const bool cpuWritable, const bool persistent, const void* const data);

	static BufferView* createTypedBufferView(const DXGI_FORMAT format, const uint64_t size, const bool createSRV, const bool createUAV, const bool createVBV, const bool createIBV, const bool cpuWritable, const bool persistent);

	BufferView* createStructuredBufferView(const uint32_t structureByteStride, const uint64_t size, const bool createSRV, const bool createUAV, const bool createVBV, const bool cpuWritable, const bool persistent, const void* const data);

	static BufferView* createStructuredBufferView(const uint32_t structureByteStride, const uint64_t size, const bool createSRV, const bool createUAV, const bool createVBV, const bool cpuWritable, const bool persistent);

	BufferView* createByteAddressBufferView(const uint64_t size, const bool createSRV, const bool createUAV, const bool cpuWritable, const bool persistent, const void* const data);

	static BufferView* createByteAddressBufferView(const uint64_t size, const bool createSRV, const bool createUAV, const bool cpuWritable, const bool persistent);

	IndexConstantBuffer* createIndexConstantBuffer(const std::initializer_list<ShaderResourceDesc>& descs, const bool cpuWritable, const bool persistent);

	static IndexConstantBuffer* createIndexConstantBuffer(const uint32_t indicesNum, const bool persistent);

	static TextureDepthView* createTextureDepthView(const uint32_t width, const uint32_t height, const DXGI_FORMAT resFormat, const uint32_t arraySize, const uint32_t mipLevels, const bool isTextureCube, const bool persistent);

	//load jpg jpeg png hdr dds(auto detect texturecube) textures
	TextureRenderView* createTextureRenderView(const std::wstring& filePath, const bool persistent,
		const DXGI_FORMAT srvFormat = DXGI_FORMAT_UNKNOWN, const DXGI_FORMAT uavFormat = DXGI_FORMAT_UNKNOWN, const DXGI_FORMAT rtvFormat = DXGI_FORMAT_UNKNOWN);

	//create texture render view from random data
	TextureRenderView* createTextureRenderView(const uint32_t width, const uint32_t height, const RandomDataType type, const bool persistent,
		const DXGI_FORMAT srvFormat = DXGI_FORMAT_UNKNOWN, const DXGI_FORMAT uavFormat = DXGI_FORMAT_UNKNOWN, const DXGI_FORMAT rtvFormat = DXGI_FORMAT_UNKNOWN);

	//create texture render view from custom parameters
	static TextureRenderView* createTextureRenderView(const uint32_t width, const uint32_t height, const DXGI_FORMAT resFormat, const uint32_t arraySize, const uint32_t mipLevels, const bool isTextureCube, const bool persistent,
		const DXGI_FORMAT srvFormat, const DXGI_FORMAT uavFormat, const DXGI_FORMAT rtvFormat, const float* const color = nullptr);

	//create texture cube from equirectangular map
	TextureRenderView* createTextureCube(const std::wstring& filePath, const uint32_t texturecubeResolution, const bool persistent,
		const DXGI_FORMAT srvFormat = DXGI_FORMAT_UNKNOWN, const DXGI_FORMAT uavFormat = DXGI_FORMAT_UNKNOWN, const DXGI_FORMAT rtvFormat = DXGI_FORMAT_UNKNOWN);

	//create texture cube from 6 seperate textures
	TextureRenderView* createTextureCube(const std::initializer_list<std::wstring>& texturesPath, const bool persistent,
		const DXGI_FORMAT srvFormat = DXGI_FORMAT_UNKNOWN, const DXGI_FORMAT uavFormat = DXGI_FORMAT_UNKNOWN, const DXGI_FORMAT rtvFormat = DXGI_FORMAT_UNKNOWN);

private:

	std::vector<Resource*>* resources;

	std::vector<EngineResource*>* engineResources;

	//high level task such as create texture cube from equirectangular map
	GraphicsContext* const context;

	//low level task such as initalize texture with data
	CommandList* const commandList;

};

#endif // !_RESOURCEMANAGER_H_