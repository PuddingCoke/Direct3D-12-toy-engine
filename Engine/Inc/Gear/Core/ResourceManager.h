#pragma once

#ifndef _CORE_RESOURCEMANAGER_H_
#define _CORE_RESOURCEMANAGER_H_

#include<Gear/Core/GraphicsContext.h>

#include<Gear/Core/Resource/SwapTexture.h>

#include<Gear/Core/Resource/ImmutableCBuffer.h>

#include<Gear/Core/Resource/StaticCBuffer.h>

#include<Gear/Core/Resource/DynamicCbuffer.h>

#include<Gear/Core/Resource/BufferView.h>

#include<Gear/Core/Resource/TextureRenderView.h>

#include<Gear/Core/Resource/TextureDepthView.h>

namespace Core
{
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

		ResourceManager(const ResourceManager&) = delete;

		void operator=(const ResourceManager&) = delete;

		ResourceManager();

		~ResourceManager();

		void deferredRelease(Resource::D3D12Resource::D3D12ResourceBase* const resource);

		void deferredRelease(Resource::EngineResource* const engineResource);

		void cleanTransientResources();

		GraphicsContext* getGraphicsContext() const;

		D3D12Core::CommandList* getCommandList() const;

		//create low level resource

		//create buffer from data
		Resource::D3D12Resource::Buffer* createBuffer(const void* const data, const uint64_t size, const D3D12_RESOURCE_FLAGS resFlags);

		//create texture from file
		Resource::D3D12Resource::Texture* createTexture(const std::wstring& filePath, const D3D12_RESOURCE_FLAGS resFlags, bool* const isTextureCube);

		//create texture from random data
		Resource::D3D12Resource::Texture* createTexture(const uint32_t width, const uint32_t height, const RandomDataType type, const D3D12_RESOURCE_FLAGS resFlags);

		//create high level resource

		//create immutable constant buffer
		Resource::ImmutableCBuffer* createImmutableCBuffer(const uint32_t size, const void* const data, const bool persistent);

		//create static constant buffer with data
		Resource::StaticCBuffer* createStaticCBuffer(const uint32_t size, const void* const data, const bool persistent);

		//create static constant buffer without data
		static Resource::StaticCBuffer* createStaticCBuffer(const uint32_t size, const bool persistent);

		//create dynamic constant buffer
		static Resource::DynamicCBuffer* createDynamicCBuffer(const uint32_t size, const void* const data = nullptr);

		Resource::BufferView* createTypedBufferView(const DXGI_FORMAT format, const uint64_t size, const bool createSRV, const bool createUAV, const bool createVBV, const bool createIBV, const bool cpuWritable, const bool persistent, const void* const data);

		static Resource::BufferView* createTypedBufferView(const DXGI_FORMAT format, const uint64_t size, const bool createSRV, const bool createUAV, const bool createVBV, const bool createIBV, const bool cpuWritable, const bool persistent);

		Resource::BufferView* createStructuredBufferView(const uint32_t structureByteStride, const uint64_t size, const bool createSRV, const bool createUAV, const bool createVBV, const bool cpuWritable, const bool persistent, const void* const data);

		static Resource::BufferView* createStructuredBufferView(const uint32_t structureByteStride, const uint64_t size, const bool createSRV, const bool createUAV, const bool createVBV, const bool cpuWritable, const bool persistent);

		Resource::BufferView* createByteAddressBufferView(const uint64_t size, const bool createSRV, const bool createUAV, const bool cpuWritable, const bool persistent, const void* const data);

		static Resource::BufferView* createByteAddressBufferView(const uint64_t size, const bool createSRV, const bool createUAV, const bool cpuWritable, const bool persistent);

		static Resource::TextureDepthView* createTextureDepthView(const uint32_t width, const uint32_t height, const DXGI_FORMAT resFormat, const uint32_t arraySize, const uint32_t mipLevels, const bool isTextureCube, const bool persistent);

		//load jpg jpeg png hdr dds(auto detect texturecube) textures
		Resource::TextureRenderView* createTextureRenderView(const std::wstring& filePath, const bool persistent, const bool hasUAV = false, const bool hasRTV = false);

		//create texture render view from random data
		Resource::TextureRenderView* createTextureRenderView(const uint32_t width, const uint32_t height, const RandomDataType type, const bool persistent,
			const DXGI_FORMAT srvFormat = DXGI_FORMAT_UNKNOWN, const DXGI_FORMAT uavFormat = DXGI_FORMAT_UNKNOWN, const DXGI_FORMAT rtvFormat = DXGI_FORMAT_UNKNOWN);

		//create texture render view from custom parameters
		static Resource::TextureRenderView* createTextureRenderView(const uint32_t width, const uint32_t height, const DXGI_FORMAT resFormat, const uint32_t arraySize, const uint32_t mipLevels, const bool isTextureCube, const bool persistent,
			const DXGI_FORMAT srvFormat, const DXGI_FORMAT uavFormat, const DXGI_FORMAT rtvFormat, const float* const color = nullptr);

		//create texture cube from equirectangular map
		Resource::TextureRenderView* createTextureCube(const std::wstring& filePath, const uint32_t texturecubeResolution, const bool persistent, const bool hasUAV = false, const bool hasRTV = false);

		//create texture cube from 6 seperate textures
		Resource::TextureRenderView* createTextureCube(const std::initializer_list<std::wstring>& texturesPath, const bool persistent,
			const DXGI_FORMAT srvFormat = DXGI_FORMAT_UNKNOWN, const DXGI_FORMAT uavFormat = DXGI_FORMAT_UNKNOWN, const DXGI_FORMAT rtvFormat = DXGI_FORMAT_UNKNOWN);

	protected:

		//high level task such as create texture cube from equirectangular map
		GraphicsContext* const context;

		//low level task such as initalize texture with data
		D3D12Core::CommandList* const commandList;

	private:

		std::vector<Resource::D3D12Resource::D3D12ResourceBase*>* resources;

		std::vector<Resource::EngineResource*>* engineResources;

	};
}

#endif // !_CORE_RESOURCEMANAGER_H_