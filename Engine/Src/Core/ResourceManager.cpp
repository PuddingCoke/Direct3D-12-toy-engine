#include<Gear/Core/ResourceManager.h>

#include<Gear/Core/Graphics.h>

#include<Gear/Core/StaticEffect/HDRClampEffect.h>

#include<Gear/Core/StaticEffect/LatLongMapToCubeMapEffect.h>

#include<Gear/Utils/Math.h>

#include<Gear/Utils/Random.h>

#include<Gear/Utils/Utils.h>

#include<DirectXTex/DDSTextureLoader12.h>

#include<DirectXTex/WICTextureLoader12.h>

#include<DirectXTex/DirectXTexEXR.h>

#include<DirectXTex/DirectXTex.h>

ResourceManager::ResourceManager() :
	context(new GraphicsContext()), commandList(context->getCommandList()),
	resources(new std::vector<Resource*>[Graphics::getFrameBufferCount()]),
	engineResources(new std::vector<EngineResource*>[Graphics::getFrameBufferCount()])
{
}

ResourceManager::~ResourceManager()
{
	for (uint32_t i = 0; i < Graphics::getFrameBufferCount(); i++)
	{
		for (const Resource* const resource : resources[i])
		{
			delete resource;
		}

		resources[i].clear();

		for (const EngineResource* const engineResource : engineResources[i])
		{
			delete engineResource;
		}

		engineResources[i].clear();
	}

	delete[] resources;

	delete[] engineResources;

	delete context;
}

void ResourceManager::deferredRelease(Resource* const resource)
{
	resources[Graphics::getFrameIndex()].push_back(resource);
}

void ResourceManager::deferredRelease(EngineResource* const engineResource)
{
	engineResources[Graphics::getFrameIndex()].push_back(engineResource);
}

void ResourceManager::cleanTransientResources()
{
	for (const Resource* const resource : resources[Graphics::getFrameIndex()])
	{
		delete resource;
	}

	resources[Graphics::getFrameIndex()].clear();

	for (const EngineResource* const engineResource : engineResources[Graphics::getFrameIndex()])
	{
		delete engineResource;
	}

	engineResources[Graphics::getFrameIndex()].clear();
}

GraphicsContext* ResourceManager::getGraphicsContext() const
{
	return context;
}

CommandList* ResourceManager::getCommandList() const
{
	return commandList;
}

Buffer* ResourceManager::createBuffer(const void* const data, const uint64_t size, const D3D12_RESOURCE_FLAGS resFlags)
{
	Buffer* buffer = new Buffer(size, true, resFlags);

	UploadHeap* uploadHeap = new UploadHeap(size);

	uploadHeap->update(data, size);

	deferredRelease(uploadHeap);

	commandList->copyBufferRegion(buffer, 0, uploadHeap, 0, size);

	return buffer;
}

Texture* ResourceManager::createTexture(const std::wstring& filePath, const D3D12_RESOURCE_FLAGS resFlags, bool* const isTextureCube)
{
	Texture* texture = nullptr;

	const std::wstring fileExtension = Utils::File::getExtension(filePath);

	if (isTextureCube)
	{
		*isTextureCube = false;
	}

	if (fileExtension == L"bmp" || fileExtension == L"jpg" || fileExtension == L"jpeg" || fileExtension == L"png" || fileExtension == L"tiff")
	{
		std::unique_ptr<uint8_t[]> decodedData;

		D3D12_SUBRESOURCE_DATA subresource;

		ComPtr<ID3D12Resource> tex;

		CHECKERROR(DirectX::LoadWICTextureFromFileEx(GraphicsDevice::get(), filePath.c_str(), 0, resFlags, DirectX::WIC_LOADER_DEFAULT, &tex, decodedData, subresource));

		texture = new Texture(tex, true, D3D12_RESOURCE_STATE_COPY_DEST);

		const uint64_t uploadHeapSize = GetRequiredIntermediateSize(texture->getResource(), 0, 1);

		UploadHeap* const uploadHeap = new UploadHeap(uploadHeapSize);

		deferredRelease(uploadHeap);

		UpdateSubresources(commandList->get(), texture->getResource(), uploadHeap->getResource(), 0, 0, 1, &subresource);
	}
	else if (fileExtension == L"dds")
	{
		std::unique_ptr<uint8_t[]> decodedData;

		std::vector<D3D12_SUBRESOURCE_DATA> subresources;

		ComPtr<ID3D12Resource> tex;

		CHECKERROR(DirectX::LoadDDSTextureFromFileEx(GraphicsDevice::get(), filePath.c_str(), 0, resFlags, DirectX::DDS_LOADER_DEFAULT, &tex, decodedData, subresources, nullptr, isTextureCube));

		texture = new Texture(tex, true, D3D12_RESOURCE_STATE_COPY_DEST);

		const uint64_t uploadHeapSize = GetRequiredIntermediateSize(texture->getResource(), 0, static_cast<uint32_t>(subresources.size()));

		UploadHeap* const uploadHeap = new UploadHeap(uploadHeapSize);

		deferredRelease(uploadHeap);

		UpdateSubresources(commandList->get(), texture->getResource(), uploadHeap->getResource(), 0, 0, static_cast<uint32_t>(subresources.size()), subresources.data());
	}
	else if (fileExtension == L"hdr" || fileExtension == L"tga" || fileExtension == L"exr")
	{
		DirectX::TexMetadata metadata;

		DirectX::ScratchImage scratchImage;

		if (fileExtension == L"hdr")
		{
			CHECKERROR(DirectX::LoadFromHDRFile(filePath.c_str(), &metadata, scratchImage));
		}
		else if (fileExtension == L"tga")
		{
			CHECKERROR(DirectX::LoadFromTGAFile(filePath.c_str(), &metadata, scratchImage));
		}
		else
		{
			CHECKERROR(DirectX::LoadFromEXRFile(filePath.c_str(), &metadata, scratchImage));
		}

		const DirectX::Image* image = scratchImage.GetImage(0, 0, 0);

		D3D12_SUBRESOURCE_DATA subresource;
		subresource.pData = image->pixels;
		subresource.RowPitch = image->rowPitch;
		subresource.SlicePitch = image->slicePitch;

		ComPtr<ID3D12Resource> tex;

		CHECKERROR(DirectX::CreateTextureEx(GraphicsDevice::get(), metadata, resFlags, DirectX::CREATETEX_DEFAULT, &tex));

		texture = new Texture(tex, true, D3D12_RESOURCE_STATE_COPY_DEST);

		const uint64_t uploadHeapSize = GetRequiredIntermediateSize(texture->getResource(), 0, 1);

		UploadHeap* const uploadHeap = new UploadHeap(uploadHeapSize);

		deferredRelease(uploadHeap);

		UpdateSubresources(commandList->get(), texture->getResource(), uploadHeap->getResource(), 0, 0, 1, &subresource);
	}
	else
	{
		LOGERROR(fileExtension, L"is not supported");
	}

	LOGSUCCESS(L"load texture from", LogColor::brightBlue, filePath, LogColor::defaultColor, L"succeeded");

	return texture;
}

Texture* ResourceManager::createTexture(const uint32_t width, const uint32_t height, const RandomDataType type, const D3D12_RESOURCE_FLAGS resFlags)
{
	Texture* texture = nullptr;

	if (type == RandomDataType::NOISE)
	{
		struct Col
		{
			uint8_t r, g, b, a;
		};

		std::vector<Col> colors(width * height);

		for (uint32_t i = 0; i < width * height; i++)
		{
			colors[i] =
			{
				static_cast<uint8_t>(Random::genUint() % 256u),
				static_cast<uint8_t>(Random::genUint() % 256u),
				static_cast<uint8_t>(Random::genUint() % 256u),
				static_cast<uint8_t>(Random::genUint() % 256u)
			};
		}

		texture = new Texture(width, height, DXGI_FORMAT_R8G8B8A8_UNORM, 1, 1, true, resFlags);

		const uint64_t uploadHeapSize = GetRequiredIntermediateSize(texture->getResource(), 0, 1);

		UploadHeap* uploadHeap = new UploadHeap(uploadHeapSize);

		deferredRelease(uploadHeap);

		D3D12_SUBRESOURCE_DATA subresourceData = {};
		subresourceData.pData = colors.data();
		subresourceData.RowPitch = width * 4u;
		subresourceData.SlicePitch = subresourceData.RowPitch * height;

		UpdateSubresources(commandList->get(), texture->getResource(), uploadHeap->getResource(), 0, 0, 1, &subresourceData);
	}
	else
	{
		struct Col
		{
			DirectX::PackedVector::HALF r, g, b, a;
		};

		std::vector<Col> colors(width * height);

		for (uint32_t i = 0; i < width * height; i++)
		{
			colors[i] =
			{
				DirectX::PackedVector::XMConvertFloatToHalf(Random::genGauss()),
				DirectX::PackedVector::XMConvertFloatToHalf(Random::genGauss()),
				DirectX::PackedVector::XMConvertFloatToHalf(Random::genGauss()),
				DirectX::PackedVector::XMConvertFloatToHalf(Random::genGauss())
			};
		}

		texture = new Texture(width, height, DXGI_FORMAT_R16G16B16A16_FLOAT, 1, 1, true, resFlags);

		const uint64_t uploadHeapSize = GetRequiredIntermediateSize(texture->getResource(), 0, 1);

		UploadHeap* uploadHeap = new UploadHeap(uploadHeapSize);

		deferredRelease(uploadHeap);

		D3D12_SUBRESOURCE_DATA subresourceData = {};
		subresourceData.pData = colors.data();
		subresourceData.RowPitch = width * 8u;
		subresourceData.SlicePitch = subresourceData.RowPitch * height;

		UpdateSubresources(commandList->get(), texture->getResource(), uploadHeap->getResource(), 0, 0, 1, &subresourceData);
	}

	return texture;
}

ConstantBuffer* ResourceManager::createConstantBuffer(const uint32_t size, const bool cpuWritable, const void* const data, const bool persistent)
{
	ConstantBuffer* constantBuffer = nullptr;

	if (!cpuWritable)
	{
		Buffer* buffer = createBuffer(data, size, D3D12_RESOURCE_FLAG_NONE);

		commandList->pushResourceTrackList(buffer);

		buffer->setState(D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);

		commandList->transitionResources();

		buffer->setStateTracking(false);

		constantBuffer = new ConstantBuffer(buffer, size, persistent);
	}
	else
	{
		constantBuffer = new ConstantBuffer(nullptr, size, persistent);

		if (data)
		{
			constantBuffer->update(data, size);
		}
	}

	return constantBuffer;
}

ConstantBuffer* ResourceManager::createConstantBuffer(const uint32_t size, const bool persistent)
{
	return new ConstantBuffer(nullptr, size, persistent);
}

BufferView* ResourceManager::createTypedBufferView(const DXGI_FORMAT format, const uint64_t size, const bool createSRV, const bool createUAV, const bool createVBV, const bool createIBV, const bool cpuWritable, const bool persistent, const void* const data)
{
	if (createVBV && createIBV)
	{
		LOGERROR(L"a buffer cannot be used as VBV and IBV at the same time");
	}

	D3D12_RESOURCE_FLAGS resFlags = D3D12_RESOURCE_FLAG_NONE;

	if (createUAV)
	{
		resFlags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
	}

	Buffer* const buffer = createBuffer(data, size, resFlags);

	if (!cpuWritable && !createUAV)
	{
		uint32_t finalState = 0;

		if (createSRV)
		{
			finalState |= D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE;
		}

		if (createVBV)
		{
			finalState |= D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
		}

		if (createIBV)
		{
			finalState |= D3D12_RESOURCE_STATE_INDEX_BUFFER;
		}

		commandList->pushResourceTrackList(buffer);

		buffer->setState(finalState);

		commandList->transitionResources();

		buffer->setStateTracking(false);
	}

	return new BufferView(buffer, 0, format, size, createSRV, createUAV, createVBV, createIBV, cpuWritable, persistent);
}

BufferView* ResourceManager::createTypedBufferView(const DXGI_FORMAT format, const uint64_t size, const bool createSRV, const bool createUAV, const bool createVBV, const bool createIBV, const bool cpuWritable, const bool persistent)
{
	if (createVBV && createIBV)
	{
		LOGERROR(L"a bufffer cannot be used as VBV and IBV at the same time");
	}

	D3D12_RESOURCE_FLAGS resFlags = D3D12_RESOURCE_FLAG_NONE;

	if (createUAV)
	{
		resFlags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
	}

	Buffer* const buffer = new Buffer(size, true, resFlags);

	return new BufferView(buffer, 0, format, size, createSRV, createUAV, createVBV, createIBV, cpuWritable, persistent);
}

BufferView* ResourceManager::createStructuredBufferView(const uint32_t structureByteStride, const uint64_t size, const bool createSRV, const bool createUAV, const bool createVBV, const bool cpuWritable, const bool persistent, const void* const data)
{
	D3D12_RESOURCE_FLAGS resFlags = D3D12_RESOURCE_FLAG_NONE;

	if (createUAV)
	{
		resFlags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
	}

	Buffer* const buffer = createBuffer(data, size, resFlags);

	if (!cpuWritable && !createUAV)
	{
		uint32_t finalState = 0;

		if (createSRV)
		{
			finalState |= D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE;
		}

		if (createVBV)
		{
			finalState |= D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
		}

		commandList->pushResourceTrackList(buffer);

		buffer->setState(finalState);

		commandList->transitionResources();

		buffer->setStateTracking(false);
	}

	return new BufferView(buffer, structureByteStride, DXGI_FORMAT_UNKNOWN, size, createSRV, createUAV, createVBV, false, cpuWritable, persistent);
}

BufferView* ResourceManager::createStructuredBufferView(const uint32_t structureByteStride, const uint64_t size, const bool createSRV, const bool createUAV, const bool createVBV, const bool cpuWritable, const bool persistent)
{
	D3D12_RESOURCE_FLAGS resFlags = D3D12_RESOURCE_FLAG_NONE;

	if (createUAV)
	{
		resFlags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
	}

	Buffer* const buffer = new Buffer(size, true, resFlags);

	return new BufferView(buffer, structureByteStride, DXGI_FORMAT_UNKNOWN, size, createSRV, createUAV, createVBV, false, cpuWritable, persistent);
}

BufferView* ResourceManager::createByteAddressBufferView(const uint64_t size, const bool createSRV, const bool createUAV, const bool cpuWritable, const bool persistent, const void* const data)
{
	D3D12_RESOURCE_FLAGS resFlags = D3D12_RESOURCE_FLAG_NONE;

	if (createUAV)
	{
		resFlags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
	}

	Buffer* const buffer = createBuffer(data, size, resFlags);

	if (!cpuWritable && !createUAV)
	{
		uint32_t finalState = 0;

		if (createSRV)
		{
			finalState |= D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE;
		}

		commandList->pushResourceTrackList(buffer);

		buffer->setState(finalState);

		commandList->transitionResources();

		buffer->setStateTracking(false);
	}

	return new BufferView(buffer, 0, DXGI_FORMAT_UNKNOWN, size, createSRV, createUAV, false, false, cpuWritable, persistent);
}

BufferView* ResourceManager::createByteAddressBufferView(const uint64_t size, const bool createSRV, const bool createUAV, const bool cpuWritable, const bool persistent)
{
	D3D12_RESOURCE_FLAGS resFlags = D3D12_RESOURCE_FLAG_NONE;

	if (createUAV)
	{
		resFlags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
	}

	Buffer* const buffer = new Buffer(size, true, resFlags);

	return new BufferView(buffer, 0, DXGI_FORMAT_UNKNOWN, size, createSRV, createUAV, false, false, cpuWritable, persistent);
}

IndexConstantBuffer* ResourceManager::createIndexConstantBuffer(const std::initializer_list<ShaderResourceDesc>& descs, const bool cpuWritable, const bool persistent)
{
	const uint32_t alignedIndicesNum = ((descs.size() + 63) & ~63);

	std::vector<uint32_t> indices = std::vector<uint32_t>(alignedIndicesNum);

	{
		uint32_t index = 0;

		for (const ShaderResourceDesc& desc : descs)
		{
			indices[index] = desc.resourceIndex;

			index++;
		}
	}

	ConstantBuffer* constantBuffer = createConstantBuffer(sizeof(uint32_t) * alignedIndicesNum, cpuWritable, indices.data(), persistent);

	return new IndexConstantBuffer(constantBuffer, descs);
}

IndexConstantBuffer* ResourceManager::createIndexConstantBuffer(const uint32_t indicesNum, const bool persistent)
{
	const uint32_t alignedIndicesNum = ((indicesNum + 63) & ~63);

	ConstantBuffer* constantBuffer = createConstantBuffer(alignedIndicesNum * sizeof(uint32_t), persistent);

	return new IndexConstantBuffer(constantBuffer);
}

TextureDepthView* ResourceManager::createTextureDepthView(const uint32_t width, const uint32_t height, const DXGI_FORMAT resFormat, const uint32_t arraySize, const uint32_t mipLevels, const bool isTextureCube, const bool persistent)
{
	DXGI_FORMAT clearValueFormat = DXGI_FORMAT_UNKNOWN;

	switch (resFormat)
	{
	case DXGI_FORMAT_D32_FLOAT:
	case DXGI_FORMAT_R32_TYPELESS:
		clearValueFormat = DXGI_FORMAT_D32_FLOAT;
		break;
	case DXGI_FORMAT_D16_UNORM:
	case DXGI_FORMAT_R16_TYPELESS:
		clearValueFormat = DXGI_FORMAT_D16_UNORM;
		break;
	case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
	case DXGI_FORMAT_R32G8X24_TYPELESS:
		clearValueFormat = DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
		break;
	case DXGI_FORMAT_D24_UNORM_S8_UINT:
	case DXGI_FORMAT_R24G8_TYPELESS:
		clearValueFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
		break;
	default:
		LOGERROR(L"not supported dsv format");
		break;
	}

	D3D12_CLEAR_VALUE clearValue = {};
	clearValue.Format = clearValueFormat;
	clearValue.DepthStencil.Depth = 1.f;
	clearValue.DepthStencil.Stencil = 0;

	Texture* texture = new Texture(width, height, resFormat, arraySize, mipLevels, true, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL, &clearValue);

	return new TextureDepthView(texture, isTextureCube, persistent);
}

TextureRenderView* ResourceManager::createTextureRenderView(const std::wstring& filePath, const bool persistent, const bool hasUAV, const bool hasRTV)
{
	bool stateTracking = true;

	D3D12_RESOURCE_FLAGS resFlags = D3D12_RESOURCE_FLAG_NONE;

	if ((!hasRTV) && (!hasUAV))
	{
		stateTracking = false;
	}
	else
	{
		if (hasRTV)
		{
			resFlags |= D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
		}

		if (hasUAV)
		{
			resFlags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
		}
	}

	bool isTextureCube = false;

	Texture* const texture = createTexture(filePath, resFlags, &isTextureCube);

	if (!stateTracking)
	{
		commandList->pushResourceTrackList(texture);

		texture->setAllState(D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE);

		commandList->transitionResources();

		texture->setStateTracking(false);
	}

	if (stateTracking)
	{
		return new TextureRenderView(texture, isTextureCube, persistent, texture->getFormat(),
			hasUAV ? texture->getFormat() : DXGI_FORMAT_UNKNOWN, hasRTV ? texture->getFormat() : DXGI_FORMAT_UNKNOWN);
	}
	else
	{
		return new TextureRenderView(texture, isTextureCube, persistent, texture->getFormat(),
			DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_UNKNOWN);
	}
}

TextureRenderView* ResourceManager::createTextureRenderView(const uint32_t width, const uint32_t height, const RandomDataType type, const bool persistent, const DXGI_FORMAT srvFormat, const DXGI_FORMAT uavFormat, const DXGI_FORMAT rtvFormat)
{
	const bool hasRTV = (rtvFormat != DXGI_FORMAT_UNKNOWN);

	const bool hasUAV = (uavFormat != DXGI_FORMAT_UNKNOWN);

	bool stateTracking = true;

	D3D12_RESOURCE_FLAGS resFlags = D3D12_RESOURCE_FLAG_NONE;

	if ((!hasRTV) && (!hasUAV))
	{
		stateTracking = false;
	}
	else
	{
		if (hasRTV)
		{
			resFlags |= D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
		}

		if (hasUAV)
		{
			resFlags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
		}
	}

	Texture* texture = createTexture(width, height, type, resFlags);

	if (!stateTracking)
	{
		commandList->pushResourceTrackList(texture);

		texture->setAllState(D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE);

		commandList->transitionResources();

		texture->setStateTracking(false);
	}

	if (srvFormat == DXGI_FORMAT_UNKNOWN)
	{
		return new TextureRenderView(texture, false, persistent, texture->getFormat(), DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_UNKNOWN);
	}
	else
	{
		return new TextureRenderView(texture, false, persistent, srvFormat, uavFormat, rtvFormat);
	}
}

TextureRenderView* ResourceManager::createTextureRenderView(const uint32_t width, const uint32_t height, const DXGI_FORMAT resFormat, const uint32_t arraySize, const uint32_t mipLevels, const bool isTextureCube, const bool persistent, const DXGI_FORMAT srvFormat, const DXGI_FORMAT uavFormat, const DXGI_FORMAT rtvFormat, const float* const color)
{
	const bool hasRTV = (rtvFormat != DXGI_FORMAT_UNKNOWN);

	const bool hasUAV = (uavFormat != DXGI_FORMAT_UNKNOWN);

	if ((!hasRTV) && (!hasUAV))
	{
		LOGERROR(L"you must set UAV or RTV format for customized render texture view");
	}
	else if (srvFormat == DXGI_FORMAT_UNKNOWN)
	{
		LOGERROR(L"customized render texture view must have a valid SRV format");
	}

	D3D12_RESOURCE_FLAGS resFlags = D3D12_RESOURCE_FLAG_NONE;

	if (hasRTV)
	{
		resFlags |= D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
	}

	if (hasUAV)
	{
		resFlags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
	}

	Texture* texture = nullptr;

	if (color)
	{
		D3D12_CLEAR_VALUE clearValue = {};
		clearValue.Format = rtvFormat;
		memcpy(clearValue.Color, color, sizeof(float) * 4);

		texture = new Texture(width, height, resFormat, arraySize, mipLevels, true, resFlags, &clearValue);
	}
	else
	{
		texture = new Texture(width, height, resFormat, arraySize, mipLevels, true, resFlags);
	}

	return new TextureRenderView(texture, isTextureCube, persistent, srvFormat, uavFormat, rtvFormat);
}

TextureRenderView* ResourceManager::createTextureCube(const std::wstring& filePath, const uint32_t texturecubeResolution, const bool persistent, const bool hasUAV, const bool hasRTV)
{
	TextureRenderView* const equirectangularMap = createTextureRenderView(filePath, false, true, false);

	equirectangularMap->copyDescriptors();

	deferredRelease(equirectangularMap);

	HDRClampEffect::get()->process(context, equirectangularMap);

	DXGI_FORMAT resFormat = DXGI_FORMAT_UNKNOWN;

	switch (Utils::getPixelSize(equirectangularMap->getTexture()->getFormat()))
	{
	case 4:
		resFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
		break;
	case 8:
		resFormat = DXGI_FORMAT_R16G16B16A16_FLOAT;
		break;
	case 16:
		resFormat = DXGI_FORMAT_R32G32B32A32_FLOAT;
		break;
	default:
		LOGERROR(L"not supported equirectangular texture format");
		break;
	}

	TextureRenderView* const cubeMap = createTextureRenderView(texturecubeResolution, texturecubeResolution, resFormat, 6, 1, true, false,
		resFormat, DXGI_FORMAT_UNKNOWN, resFormat);

	deferredRelease(cubeMap);

	LatLongMapToCubeMapEffect::get()->process(context, equirectangularMap, cubeMap);

	bool stateTracking = true;

	D3D12_RESOURCE_FLAGS resFlags = D3D12_RESOURCE_FLAG_NONE;

	if ((!hasRTV) && (!hasUAV))
	{
		stateTracking = false;
	}
	else
	{
		if (hasRTV)
		{
			resFlags |= D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
		}

		if (hasUAV)
		{
			resFlags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
		}
	}

	Texture* dstTexture = new Texture(texturecubeResolution, texturecubeResolution, resFormat, 6, 1, true, resFlags);

	Texture* srcTexture = cubeMap->getTexture();

	for (uint32_t i = 0; i < 6; i++)
	{
		const uint32_t dstSubresource = D3D12CalcSubresource(0, i, 0, dstTexture->getMipLevels(), dstTexture->getArraySize());

		const uint32_t srcSubresource = D3D12CalcSubresource(0, i, 0, srcTexture->getMipLevels(), srcTexture->getArraySize());

		commandList->copyTextureRegion(dstTexture, dstSubresource, srcTexture, srcSubresource);
	}

	if (!stateTracking)
	{
		commandList->pushResourceTrackList(dstTexture);

		dstTexture->setAllState(D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE);

		commandList->transitionResources();

		dstTexture->setStateTracking(false);
	}

	if (stateTracking)
	{
		return new TextureRenderView(dstTexture, true, persistent, dstTexture->getFormat(),
			hasUAV ? dstTexture->getFormat() : DXGI_FORMAT_UNKNOWN, hasRTV ? dstTexture->getFormat() : DXGI_FORMAT_UNKNOWN);
	}
	else
	{
		return new TextureRenderView(dstTexture, true, persistent, dstTexture->getFormat(),
			DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_UNKNOWN);
	}
}

TextureRenderView* ResourceManager::createTextureCube(const std::initializer_list<std::wstring>& texturesPath, const bool persistent, const DXGI_FORMAT srvFormat, const DXGI_FORMAT uavFormat, const DXGI_FORMAT rtvFormat)
{
	Texture* srcTextures[6] = {};

	{
		uint32_t index = 0;

		for (const std::wstring& filePath : texturesPath)
		{
			srcTextures[index] = createTexture(filePath, D3D12_RESOURCE_FLAG_NONE, nullptr);

			deferredRelease(srcTextures[index]);

			index++;
		}
	}

	const bool hasRTV = (rtvFormat != DXGI_FORMAT_UNKNOWN);

	const bool hasUAV = (uavFormat != DXGI_FORMAT_UNKNOWN);

	bool stateTracking = true;

	D3D12_RESOURCE_FLAGS resFlags = D3D12_RESOURCE_FLAG_NONE;

	if ((!hasRTV) && (!hasUAV))
	{
		stateTracking = false;
	}
	else
	{
		if (hasRTV)
		{
			resFlags |= D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
		}

		if (hasUAV)
		{
			resFlags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
		}
	}

	Texture* dstTexture = new Texture(srcTextures[0]->getWidth(), srcTextures[0]->getHeight(), srcTextures[0]->getFormat(), 6, 1, true, resFlags);

	for (uint32_t i = 0; i < 6; i++)
	{
		const uint32_t dstSubresource = D3D12CalcSubresource(0, i, 0, dstTexture->getMipLevels(), dstTexture->getArraySize());

		const uint32_t srcSubresource = D3D12CalcSubresource(0, 0, 0, srcTextures[i]->getMipLevels(), srcTextures[i]->getArraySize());

		commandList->copyTextureRegion(dstTexture, dstSubresource, srcTextures[i], srcSubresource);
	}

	if (!stateTracking)
	{
		commandList->pushResourceTrackList(dstTexture);

		dstTexture->setAllState(D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE);

		commandList->transitionResources();

		dstTexture->setStateTracking(false);
	}

	if (srvFormat == DXGI_FORMAT_UNKNOWN)
	{
		return new TextureRenderView(dstTexture, true, persistent, dstTexture->getFormat(), DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_UNKNOWN);
	}
	else
	{
		return new TextureRenderView(dstTexture, true, persistent, srvFormat, uavFormat, rtvFormat);
	}
}
