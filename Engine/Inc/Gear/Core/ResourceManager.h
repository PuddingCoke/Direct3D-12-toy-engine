#pragma once

#ifndef _RESOURCEMANAGER_H_
#define _RESOURCEMANAGER_H_

#include<stb_image/stb_image.h>
#include<DDSTextureLoader/DDSTextureLoader12.h>

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

	//create buffer from data
	Buffer* createBuffer(const void* const data, const UINT size, const D3D12_RESOURCE_FLAGS resFlags);

	//create texture from file
	Texture* createTexture(const std::string& filePath, const D3D12_RESOURCE_FLAGS resFlags, bool* const isTextureCube);

	//create texture from random data
	Texture* createTexture(const UINT width, const UINT height, const RandomDataType type, const D3D12_RESOURCE_FLAGS resFlags);

	//following methods create high level resources

	ConstantBuffer* createConstantBuffer(const UINT size, const bool cpuWritable, const void* const data, const bool persistent);

	static ConstantBuffer* createConstantBuffer(const UINT size, const bool persistent);

	BufferView* createTypedBufferView(const DXGI_FORMAT format, const UINT size, const bool createSRV, const bool createUAV, const bool createVBV, const bool createIBV, const bool cpuWritable, const bool persistent, const void* const data);

	static BufferView* createTypedBufferView(const DXGI_FORMAT format, const UINT size, const bool createSRV, const bool createUAV, const bool createVBV, const bool createIBV, const bool cpuWritable, const bool persistent);

	BufferView* createStructuredBufferView(const UINT structureByteStride, const UINT size, const bool createSRV, const bool createUAV, const bool createVBV, const bool cpuWritable, const bool persistent, const void* const data);

	static BufferView* createStructuredBufferView(const UINT structureByteStride, const UINT size, const bool createSRV, const bool createUAV, const bool createVBV, const bool cpuWritable, const bool persistent);

	BufferView* createByteAddressBufferView(const UINT size, const bool createSRV, const bool createUAV, const bool createVBV, const bool createIBV, const bool cpuWritable, const bool persistent, const void* const data);

	static BufferView* createByteAddressBufferView(const UINT size, const bool createSRV, const bool createUAV, const bool createVBV, const bool createIBV, const bool cpuWritable, const bool persistent);

	IndexConstantBuffer* createIndexConstantBuffer(const std::initializer_list<ShaderResourceDesc>& descs, const bool cpuWritable, const bool persistent);

	static IndexConstantBuffer* createIndexConstantBuffer(const UINT indicesNum, const bool persistent);

	static TextureDepthView* createTextureDepthView(const UINT width, const UINT height, const DXGI_FORMAT resFormat, const UINT arraySize, const UINT mipLevels, const bool isTextureCube, const bool persistent);

	//load jpg jpeg png hdr dds(auto detect texturecube) textures
	TextureRenderView* createTextureRenderView(const std::string& filePath, const bool persistent,
		const DXGI_FORMAT srvFormat = DXGI_FORMAT_UNKNOWN, const DXGI_FORMAT uavFormat = DXGI_FORMAT_UNKNOWN, const DXGI_FORMAT rtvFormat = DXGI_FORMAT_UNKNOWN);

	//create texture render view from random data
	TextureRenderView* createTextureRenderView(const UINT width, const UINT height, const RandomDataType type, const bool persistent,
		const DXGI_FORMAT srvFormat = DXGI_FORMAT_UNKNOWN, const DXGI_FORMAT uavFormat = DXGI_FORMAT_UNKNOWN, const DXGI_FORMAT rtvFormat = DXGI_FORMAT_UNKNOWN);

	//create texture render view from custom parameters
	static TextureRenderView* createTextureRenderView(const UINT width, const UINT height, const DXGI_FORMAT resFormat, const UINT arraySize, const UINT mipLevels, const bool isTextureCube, const bool persistent,
		const DXGI_FORMAT srvFormat, const DXGI_FORMAT uavFormat, const DXGI_FORMAT rtvFormat, const float* const color = nullptr);

	//create texture cube from equirectangular map
	TextureRenderView* createTextureCube(const std::string& filePath, const UINT texturecubeResolution, const bool persistent,
		const DXGI_FORMAT srvFormat = DXGI_FORMAT_UNKNOWN, const DXGI_FORMAT uavFormat = DXGI_FORMAT_UNKNOWN, const DXGI_FORMAT rtvFormat = DXGI_FORMAT_UNKNOWN);

	//create texture cube from 6 seperate textures
	TextureRenderView* createTextureCube(const std::initializer_list<std::string>& texturesPath, const bool persistent,
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
