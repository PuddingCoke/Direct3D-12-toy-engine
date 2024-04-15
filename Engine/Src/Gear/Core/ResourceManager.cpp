#include<Gear/Core/ResourceManager.h>


ResourceManager::ResourceManager(GraphicsContext* const context) :
	context(context), commandList(context->getCommandList())
{
}

ResourceManager::~ResourceManager()
{
	for (UINT i = 0; i < Graphics::FrameBufferCount; i++)
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
}

void ResourceManager::release(Resource* const resource)
{
	resources[Graphics::getFrameIndex()].push_back(resource);
}

void ResourceManager::release(EngineResource* const engineResource)
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

Buffer* ResourceManager::createBufferFromData(const void* const data, const UINT size, const D3D12_RESOURCE_FLAGS resFlags)
{
	Buffer* buffer = new Buffer(size, true, resFlags);

	UploadHeap* uploadHeap = new UploadHeap(size);

	uploadHeap->update(data, size);

	release(uploadHeap);

	commandList->copyBufferRegion(buffer, 0, uploadHeap, 0, size);

	return buffer;
}

Texture* ResourceManager::createTextureFromFile(const std::string filePath, const D3D12_RESOURCE_FLAGS resFlags, bool* const isTextureCube)
{
	Texture* texture = nullptr;

	std::string fileExtension = Utils::File::getExtension(filePath);

	for (char& c : fileExtension)
	{
		c = static_cast<char>(std::tolower(c));
	}

	if (isTextureCube)
	{
		*isTextureCube = false;
	}

	if (fileExtension == "jpg" || fileExtension == "jpeg" || fileExtension == "png")
	{
		int textureWidth, textureHeight, channels;

		unsigned char* pixels = stbi_load(filePath.c_str(), &textureWidth, &textureHeight, &channels, 4);

		const UINT width = static_cast<UINT>(textureWidth);
		const UINT height = static_cast<UINT>(textureHeight);
		const DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM;

		if (pixels)
		{
			texture = new Texture(width, height, format, 1, 1, true, resFlags);

			const UINT64 uploadHeapSize = GetRequiredIntermediateSize(texture->getResource(), 0, 1);

			UploadHeap* uploadHeap = new UploadHeap(uploadHeapSize);

			release(uploadHeap);

			D3D12_SUBRESOURCE_DATA subresourceData = {};
			subresourceData.pData = pixels;
			subresourceData.RowPitch = width * 4u;
			subresourceData.SlicePitch = subresourceData.RowPitch * height;

			UpdateSubresources(commandList->get(), texture->getResource(), uploadHeap->getResource(), 0, 0, 1, &subresourceData);

			stbi_image_free(pixels);
		}
		else
		{
			throw "Cannot open file named " + filePath;
		}
	}
	else if (fileExtension == "hdr")
	{
		int textureWidth, textureHeight, channels;

		float* pixels = stbi_loadf(filePath.c_str(), &textureWidth, &textureHeight, &channels, 4);

		const UINT width = static_cast<UINT>(textureWidth);
		const UINT height = static_cast<UINT>(textureHeight);
		const DXGI_FORMAT format = DXGI_FORMAT_R32G32B32A32_FLOAT;

		if (pixels)
		{
			texture = new Texture(width, height, format, 1, 1, true, resFlags);

			const UINT64 uploadHeapSize = GetRequiredIntermediateSize(texture->getResource(), 0, 1);

			UploadHeap* uploadHeap = new UploadHeap(uploadHeapSize);

			release(uploadHeap);

			D3D12_SUBRESOURCE_DATA subresourceData = {};
			subresourceData.pData = pixels;
			subresourceData.RowPitch = width * 16u;
			subresourceData.SlicePitch = subresourceData.RowPitch * height;

			UpdateSubresources(commandList->get(), texture->getResource(), uploadHeap->getResource(), 0, 0, 1, &subresourceData);

			stbi_image_free(pixels);
		}
		else
		{
			throw "Cannot open file named " + filePath;
		}
	}
	else if (fileExtension == "dds")
	{
		std::wstring wFilePath = std::wstring(filePath.begin(), filePath.end());

		std::unique_ptr<uint8_t[]> ddsData;

		std::vector<D3D12_SUBRESOURCE_DATA> subresources;

		ComPtr<ID3D12Resource> tex;

		DirectX::LoadDDSTextureFromFileEx(GraphicsDevice::get(), wFilePath.c_str(), 0, resFlags, DirectX::DDS_LOADER_DEFAULT, &tex, ddsData, subresources, nullptr, isTextureCube);

		texture = new Texture(tex, true, D3D12_RESOURCE_STATE_COPY_DEST);

		const UINT64 uploadHeapSize = GetRequiredIntermediateSize(texture->getResource(), 0, static_cast<UINT>(subresources.size()));

		UploadHeap* uploadHeap = new UploadHeap(uploadHeapSize);

		release(uploadHeap);

		UpdateSubresources(commandList->get(), texture->getResource(), uploadHeap->getResource(), 0, 0, static_cast<UINT>(subresources.size()), subresources.data());
	}
	else
	{
		throw "Format not supported";
	}

	return texture;
}

ConstantBuffer* ResourceManager::createConstantBuffer(const UINT size, const bool cpuWritable, const void* const data, const bool persistent)
{
	ConstantBuffer* constantBuffer = nullptr;

	if (!cpuWritable)
	{
		Buffer* buffer = createBufferFromData(data, size, D3D12_RESOURCE_FLAG_NONE);

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

ConstantBuffer* ResourceManager::createConstantBuffer(const UINT size, const bool persistent)
{
	return new ConstantBuffer(nullptr, size, persistent);
}

IndexBuffer* ResourceManager::createIndexBuffer(const DXGI_FORMAT format, const UINT size, const bool cpuWritable, const void* const data)
{
	Buffer* buffer = createBufferFromData(data, size, D3D12_RESOURCE_FLAG_NONE);

	if (!cpuWritable)
	{
		commandList->pushResourceTrackList(buffer);

		buffer->setState(D3D12_RESOURCE_STATE_INDEX_BUFFER);

		commandList->transitionResources();

		buffer->setStateTracking(false);
	}

	return new IndexBuffer(buffer, format, size, cpuWritable);
}

IndexBuffer* ResourceManager::createIndexBuffer(const DXGI_FORMAT format, const UINT size)
{
	Buffer* buffer = new Buffer(size, true, D3D12_RESOURCE_FLAG_NONE);

	return new IndexBuffer(buffer, format, size, true);
}

VertexBuffer* ResourceManager::createVertexBuffer(const UINT perVertexSize, const UINT size, const bool cpuWritable, const void* const data)
{
	Buffer* buffer = createBufferFromData(data, size, D3D12_RESOURCE_FLAG_NONE);

	if (!cpuWritable)
	{
		commandList->pushResourceTrackList(buffer);

		buffer->setState(D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);

		commandList->transitionResources();

		buffer->setStateTracking(false);
	}

	return new VertexBuffer(buffer, perVertexSize, size, cpuWritable);
}

VertexBuffer* ResourceManager::createVertexBuffer(const UINT perVertexSize, const UINT size)
{
	Buffer* buffer = new Buffer(size, true, D3D12_RESOURCE_FLAG_NONE);

	return new VertexBuffer(buffer, perVertexSize, size, true);
}

IndexConstantBuffer* ResourceManager::createIndexConstantBuffer(const std::initializer_list<ShaderResourceDesc>& descs, const bool cpuWritable,const bool persistent)
{
	const UINT alignedIndicesNum = ((descs.size() + 63) & ~63);

	std::vector<UINT> indices = std::vector<UINT>(alignedIndicesNum);

	{
		UINT index = 0;

		for (const ShaderResourceDesc& desc : descs)
		{
			indices[index] = desc.resourceIndex;

			index++;
		}
	}

	ConstantBuffer* constantBuffer = createConstantBuffer(sizeof(UINT) * alignedIndicesNum, cpuWritable, indices.data(), persistent);

	return new IndexConstantBuffer(constantBuffer, descs);
}

IndexConstantBuffer* ResourceManager::createIndexConstantBuffer(const UINT indicesNum,const bool persistent)
{
	const UINT alignedIndicesNum = ((indicesNum + 63) & ~63);

	ConstantBuffer* constantBuffer = createConstantBuffer(alignedIndicesNum * sizeof(UINT), persistent);

	return new IndexConstantBuffer(constantBuffer);
}

TextureDepthStencil* ResourceManager::createTextureDepthStencil(const UINT width, const UINT height, const DXGI_FORMAT resFormat, const UINT arraySize, const UINT mipLevels, const bool isTextureCube, const bool persistent)
{
	Texture* texture = new Texture(width, height, resFormat, arraySize, mipLevels, true, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);

	return new TextureDepthStencil(texture, isTextureCube, persistent);
}

TextureRenderTarget* ResourceManager::createTextureRenderTarget(const std::string filePath, const bool persistent, const DXGI_FORMAT srvFormat, const DXGI_FORMAT uavFormat, const DXGI_FORMAT rtvFormat)
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

	bool isTextureCube = false;

	Texture* texture = createTextureFromFile(filePath, resFlags, &isTextureCube);

	if (!stateTracking)
	{
		commandList->pushResourceTrackList(texture);

		texture->setAllState(D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE);

		commandList->transitionResources();

		texture->setStateTracking(false);
	}

	if (srvFormat == DXGI_FORMAT_UNKNOWN)
	{
		return new TextureRenderTarget(texture, isTextureCube, persistent, texture->getFormat(), DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_UNKNOWN);
	}
	else
	{
		return new TextureRenderTarget(texture, isTextureCube, persistent, srvFormat, uavFormat, rtvFormat);
	}
}

TextureRenderTarget* ResourceManager::createTextureRenderTarget(const UINT width, const UINT height, const DXGI_FORMAT resFormat, const UINT arraySize, const UINT mipLevels, const bool isTextureCube, const bool persistent, const DXGI_FORMAT srvFormat, const DXGI_FORMAT uavFormat, const DXGI_FORMAT rtvFormat)
{
	const bool hasRTV = (rtvFormat != DXGI_FORMAT_UNKNOWN);

	const bool hasUAV = (uavFormat != DXGI_FORMAT_UNKNOWN);

	if ((!hasRTV) && (!hasUAV))
	{
		throw "With only SRV flag set is not allowed here";
	}
	else if (srvFormat == DXGI_FORMAT_UNKNOWN)
	{
		throw "SRV format cannot be DXGI_FORMAT_UNKNOWN";
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

	Texture* texture = new Texture(width, height, resFormat, arraySize, mipLevels, true, resFlags);

	return new TextureRenderTarget(texture, isTextureCube, persistent, srvFormat, uavFormat, rtvFormat);
}
