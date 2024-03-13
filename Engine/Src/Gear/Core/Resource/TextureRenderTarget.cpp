#include<Gear/Core/Resource/TextureRenderTarget.h>

TextureRenderTarget::TextureRenderTarget(const UINT width, const UINT height, const DXGI_FORMAT resFormat, const UINT arraySize, const UINT mipLevels, const bool isTextureCube, const bool persistent,
	const DXGI_FORMAT srvFormat, const DXGI_FORMAT uavFormat, const DXGI_FORMAT rtvFormat)
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

	//stateTracking must be true because use this method to create a texture must combine SRV with other flags(UAV or RTV)
	texture = new Texture(width, height, resFormat, arraySize, mipLevels, true, resFlags);

	createViews(srvFormat, uavFormat, rtvFormat, isTextureCube, persistent);
}

TextureRenderTarget::TextureRenderTarget(const std::string filePath, ID3D12GraphicsCommandList6* commandList, std::vector<Resource*>* transientResourcePool, const bool isTextureCube, const bool persistent,
	const DXGI_FORMAT srvFormat, const DXGI_FORMAT uavFormat, const DXGI_FORMAT rtvFormat)
{
	const bool hasRTV = (rtvFormat != DXGI_FORMAT_UNKNOWN);
	const bool hasUAV = (uavFormat != DXGI_FORMAT_UNKNOWN);

	if ((!hasRTV) && (!hasUAV))
	{
		//stateTracking is disabled here because flags is set to D3D12_TEXTURE_CREATE_SRV
		texture = new Texture(filePath, commandList, transientResourcePool, false, D3D12_RESOURCE_FLAG_NONE);
	}
	else
	{
		D3D12_RESOURCE_FLAGS resFlags = D3D12_RESOURCE_FLAG_NONE;

		if (hasRTV)
		{
			resFlags |= D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
		}

		if (hasUAV)
		{
			resFlags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
		}

		texture = new Texture(filePath, commandList, transientResourcePool, true, resFlags);
	}

	if (srvFormat == DXGI_FORMAT_UNKNOWN)
	{
		createViews(texture->getFormat(), DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_UNKNOWN, isTextureCube, persistent);
	}
	else
	{
		createViews(srvFormat, uavFormat, rtvFormat, isTextureCube, persistent);
	}
}

TextureRenderTarget::TextureRenderTarget(const TextureRenderTarget& trt) :
	allSRVIndex(trt.allSRVIndex),
	srvMipIndexStart(trt.srvMipIndexStart),
	uavMipIndexStart(trt.uavMipIndexStart),
	rtvMipHandles(trt.rtvMipHandles),
	texture(new Texture(*(trt.texture)))
{
}

TextureRenderTarget::~TextureRenderTarget()
{
	if (texture)
	{
		delete texture;
	}
}

ShaderResourceDesc TextureRenderTarget::getAllSRVIndex() const
{
	ShaderResourceDesc desc = {};
	desc.type = ShaderResourceDesc::TEXTURE;
	desc.state = ShaderResourceDesc::SRV;
	desc.textureDesc.texture = texture;
	desc.textureDesc.mipSlice = D3D12_TRANSITION_ALL_MIPLEVELS;
	desc.textureDesc.resourceIndex = allSRVIndex;

	return desc;
}

ShaderResourceDesc TextureRenderTarget::getSRVMipIndex(const UINT mipSlice) const
{
	ShaderResourceDesc desc = {};
	desc.type = ShaderResourceDesc::TEXTURE;
	desc.state = ShaderResourceDesc::SRV;
	desc.textureDesc.texture = texture;
	desc.textureDesc.mipSlice = mipSlice;
	desc.textureDesc.resourceIndex = srvMipIndexStart + mipSlice;

	return desc;
}

ShaderResourceDesc TextureRenderTarget::getUAVMipIndex(const UINT mipSlice) const
{
	ShaderResourceDesc desc = {};
	desc.type = ShaderResourceDesc::TEXTURE;
	desc.state = ShaderResourceDesc::UAV;
	desc.textureDesc.texture = texture;
	desc.textureDesc.mipSlice = mipSlice;
	desc.textureDesc.resourceIndex = uavMipIndexStart + mipSlice;

	return desc;
}

RenderTargetDesc TextureRenderTarget::getRTVMipHandle(const UINT mipSlice) const
{
	RenderTargetDesc desc = {};
	desc.texture = texture;
	desc.mipSlice = mipSlice;
	desc.rtvHandle = rtvMipHandles[mipSlice];

	return desc;
}

Texture* TextureRenderTarget::getTexture() const
{
	return texture;
}

void TextureRenderTarget::copyDescriptors()
{
	const DescriptorHandle handle = GlobalDescriptorHeap::getResourceHeap()->allocDynamicDescriptor(numSRVUAVDescriptors);

	GraphicsDevice::get()->CopyDescriptorsSimple(numSRVUAVDescriptors, handle.getCPUHandle(), srvuavDescriptorHandleStart, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	allSRVIndex = handle.getCurrentIndex();

	srvMipIndexStart = allSRVIndex + 1;

	uavMipIndexStart = srvMipIndexStart + texture->getMipLevels();
}

void TextureRenderTarget::createViews(const DXGI_FORMAT srvFormat, const DXGI_FORMAT uavFormat, const DXGI_FORMAT rtvFormat, const bool isTextureCube, const bool persistent)
{
	const bool hasRTV = (rtvFormat != DXGI_FORMAT_UNKNOWN);
	const bool hasUAV = (uavFormat != DXGI_FORMAT_UNKNOWN);

	//create srv uav descriptors
	{
		//plus 1 because there is a additional srv to acess all mipslices
		numSRVUAVDescriptors = 1 + texture->getMipLevels() + static_cast<UINT>(hasUAV) * texture->getMipLevels();

		DescriptorHandle descriptorHandle = DescriptorHandle();

		if (persistent)
		{
			descriptorHandle = GlobalDescriptorHeap::getResourceHeap()->allocStaticDescriptor(numSRVUAVDescriptors);
		}
		else
		{
			descriptorHandle = GlobalDescriptorHeap::getNonShaderVisibleResourceHeap()->allocDynamicDescriptor(numSRVUAVDescriptors);

			srvuavDescriptorHandleStart = descriptorHandle.getCPUHandle();
		}

		allSRVIndex = descriptorHandle.getCurrentIndex();

		srvMipIndexStart = allSRVIndex + 1;

		uavMipIndexStart = srvMipIndexStart + texture->getMipLevels();

		std::cout << "[class TextureRenderTarget] //////////\n";
		std::cout << "[class TextureRenderTarget] stateTracking " << texture->getStateTracking() << "\n";
		std::cout << "[class TextureRenderTarget] persistent " << persistent << "\n";

		if (texture->getStateTracking())
		{
			std::cout << "[class TextureRenderTarget] uav creation flag " << hasUAV << "\n";
			std::cout << "[class TextureRenderTarget] rtv creation flag " << hasRTV << "\n";
		}

		std::cout << "[class TextureRenderTarget] isTexturecube " << isTextureCube << "\n";
		std::cout << "[class TextureRenderTarget] miplevels " << texture->getMipLevels() << "\n";
		std::cout << "[class TextureRenderTarget] arraysize " << texture->getArraySize() << "\n";
		std::cout << "[class TextureRenderTarget] all srv index " << allSRVIndex << "\n";
		std::cout << "[class TextureRenderTarget] srv slice start " << srvMipIndexStart << "\n";

		if (texture->getStateTracking())
		{
			std::cout << "[class TextureRenderTarget] uav slice start " << uavMipIndexStart << "\n";
		}

		std::cout << "[class TextureRenderTarget] //////////\n";

		if (isTextureCube) //TextureCube srv creation
		{
			const UINT cubeNum = texture->getArraySize() / 6;

			if (cubeNum > 1)
			{
				//create srv access all miplevels
				{
					D3D12_SHADER_RESOURCE_VIEW_DESC desc = {};
					desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
					desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBEARRAY;
					desc.Format = srvFormat;
					desc.TextureCubeArray.First2DArrayFace = 0;
					desc.TextureCubeArray.MipLevels = texture->getMipLevels();
					desc.TextureCubeArray.MostDetailedMip = 0;
					desc.TextureCubeArray.ResourceMinLODClamp = 0.f;
					desc.TextureCubeArray.NumCubes = cubeNum;

					GraphicsDevice::get()->CreateShaderResourceView(texture->getResource(), &desc, descriptorHandle.getCPUHandle());

					descriptorHandle.move();
				}

				//create srv acess each mipSlice
				for (UINT i = 0; i < texture->getMipLevels(); i++)
				{
					D3D12_SHADER_RESOURCE_VIEW_DESC desc = {};
					desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
					desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBEARRAY;
					desc.Format = srvFormat;
					desc.TextureCubeArray.First2DArrayFace = 0;
					desc.TextureCubeArray.MipLevels = 1;
					desc.TextureCubeArray.MostDetailedMip = i;
					desc.TextureCubeArray.ResourceMinLODClamp = 0.f;
					desc.TextureCubeArray.NumCubes = cubeNum;

					GraphicsDevice::get()->CreateShaderResourceView(texture->getResource(), &desc, descriptorHandle.getCPUHandle());

					descriptorHandle.move();
				}
			}
			else
			{
				//create srv access all miplevels
				{
					D3D12_SHADER_RESOURCE_VIEW_DESC desc = {};
					desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
					desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
					desc.Format = srvFormat;
					desc.TextureCube.MipLevels = texture->getMipLevels();
					desc.TextureCube.MostDetailedMip = 0;
					desc.TextureCube.ResourceMinLODClamp = 0.f;

					GraphicsDevice::get()->CreateShaderResourceView(texture->getResource(), &desc, descriptorHandle.getCPUHandle());

					descriptorHandle.move();
				}

				//create srv acess each mipSlice
				for (UINT i = 0; i < texture->getMipLevels(); i++)
				{
					D3D12_SHADER_RESOURCE_VIEW_DESC desc = {};
					desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
					desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
					desc.Format = srvFormat;
					desc.TextureCube.MipLevels = 1;
					desc.TextureCube.MostDetailedMip = i;
					desc.TextureCube.ResourceMinLODClamp = 0.f;

					GraphicsDevice::get()->CreateShaderResourceView(texture->getResource(), &desc, descriptorHandle.getCPUHandle());

					descriptorHandle.move();
				}
			}
		}
		else
		{
			if (texture->getArraySize() > 1)
			{
				//create srv access all miplevels
				{
					D3D12_SHADER_RESOURCE_VIEW_DESC desc = {};
					desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
					desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DARRAY;
					desc.Format = srvFormat;
					desc.Texture2DArray.ArraySize = texture->getArraySize();
					desc.Texture2DArray.FirstArraySlice = 0;
					desc.Texture2DArray.MipLevels = texture->getMipLevels();
					desc.Texture2DArray.MostDetailedMip = 0;
					desc.Texture2DArray.PlaneSlice = 0;
					desc.Texture2DArray.ResourceMinLODClamp = 0.f;

					GraphicsDevice::get()->CreateShaderResourceView(texture->getResource(), &desc, descriptorHandle.getCPUHandle());

					descriptorHandle.move();
				}

				//create srv acess each mipSlice
				for (UINT i = 0; i < texture->getMipLevels(); i++)
				{
					D3D12_SHADER_RESOURCE_VIEW_DESC desc = {};
					desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
					desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DARRAY;
					desc.Format = srvFormat;
					desc.Texture2DArray.ArraySize = texture->getArraySize();
					desc.Texture2DArray.FirstArraySlice = 0;
					desc.Texture2DArray.MipLevels = 1;
					desc.Texture2DArray.MostDetailedMip = i;
					desc.Texture2DArray.PlaneSlice = 0;
					desc.Texture2DArray.ResourceMinLODClamp = 0.f;

					GraphicsDevice::get()->CreateShaderResourceView(texture->getResource(), &desc, descriptorHandle.getCPUHandle());

					descriptorHandle.move();
				}
			}
			else
			{
				//create srv access all miplevels
				{
					D3D12_SHADER_RESOURCE_VIEW_DESC desc = {};
					desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
					desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
					desc.Format = srvFormat;
					desc.Texture2D.MipLevels = texture->getMipLevels();
					desc.Texture2D.MostDetailedMip = 0;
					desc.Texture2D.PlaneSlice = 0;
					desc.Texture2D.ResourceMinLODClamp = 0.f;

					GraphicsDevice::get()->CreateShaderResourceView(texture->getResource(), &desc, descriptorHandle.getCPUHandle());

					descriptorHandle.move();
				}

				//create srv acess each mipSlice
				for (UINT i = 0; i < texture->getMipLevels(); i++)
				{
					D3D12_SHADER_RESOURCE_VIEW_DESC desc = {};
					desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
					desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
					desc.Format = srvFormat;
					desc.Texture2D.MipLevels = 1;
					desc.Texture2D.MostDetailedMip = i;
					desc.Texture2D.PlaneSlice = 0;
					desc.Texture2D.ResourceMinLODClamp = 0.f;

					GraphicsDevice::get()->CreateShaderResourceView(texture->getResource(), &desc, descriptorHandle.getCPUHandle());

					descriptorHandle.move();
				}
			}
		}

		if (hasUAV)
		{
			if (texture->getArraySize() > 1)
			{
				for (UINT i = 0; i < texture->getMipLevels(); i++)
				{
					D3D12_UNORDERED_ACCESS_VIEW_DESC desc = {};
					desc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2DARRAY;
					desc.Format = uavFormat;
					desc.Texture2DArray.ArraySize = texture->getArraySize();
					desc.Texture2DArray.FirstArraySlice = 0;
					desc.Texture2DArray.MipSlice = i;
					desc.Texture2DArray.PlaneSlice = 0;

					GraphicsDevice::get()->CreateUnorderedAccessView(texture->getResource(), nullptr, &desc, descriptorHandle.getCPUHandle());

					descriptorHandle.move();
				}
			}
			else
			{
				for (UINT i = 0; i < texture->getMipLevels(); i++)
				{
					D3D12_UNORDERED_ACCESS_VIEW_DESC desc = {};
					desc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
					desc.Format = uavFormat;
					desc.Texture2D.MipSlice = i;
					desc.Texture2D.PlaneSlice = 0;

					GraphicsDevice::get()->CreateUnorderedAccessView(texture->getResource(), nullptr, &desc, descriptorHandle.getCPUHandle());

					descriptorHandle.move();
				}
			}
		}
	}

	//create rtv descriptor
	if (hasRTV)
	{
		DescriptorHandle descriptorHandle = DescriptorHandle();

		rtvMipHandles = std::vector<D3D12_CPU_DESCRIPTOR_HANDLE>(texture->getMipLevels());

		if (persistent)
		{
			descriptorHandle = GlobalDescriptorHeap::getRenderTargetHeap()->allocStaticDescriptor(texture->getMipLevels());
		}
		else
		{
			descriptorHandle = GlobalDescriptorHeap::getRenderTargetHeap()->allocDynamicDescriptor(texture->getMipLevels());
		}

		if (texture->getArraySize() > 1)
		{
			for (UINT i = 0; i < texture->getMipLevels(); i++)
			{
				D3D12_RENDER_TARGET_VIEW_DESC desc = {};
				desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DARRAY;
				desc.Format = rtvFormat;
				desc.Texture2DArray.ArraySize = texture->getArraySize();
				desc.Texture2DArray.FirstArraySlice = 0;
				desc.Texture2DArray.MipSlice = i;
				desc.Texture2DArray.PlaneSlice = 0;

				GraphicsDevice::get()->CreateRenderTargetView(texture->getResource(), &desc, descriptorHandle.getCPUHandle());

				rtvMipHandles[i] = descriptorHandle.getCPUHandle();

				descriptorHandle.move();
			}
		}
		else
		{
			for (UINT i = 0; i < texture->getMipLevels(); i++)
			{
				D3D12_RENDER_TARGET_VIEW_DESC desc = {};
				desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
				desc.Format = rtvFormat;
				desc.Texture2D.MipSlice = i;
				desc.Texture2D.PlaneSlice = 0;

				GraphicsDevice::get()->CreateRenderTargetView(texture->getResource(), &desc, descriptorHandle.getCPUHandle());

				rtvMipHandles[i] = descriptorHandle.getCPUHandle();

				descriptorHandle.move();
			}
		}
	}
}
