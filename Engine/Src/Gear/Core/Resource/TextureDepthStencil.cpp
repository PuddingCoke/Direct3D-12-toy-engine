#include<Gear/Core/Resource/TextureDepthStencil.h>

TextureDepthStencil::TextureDepthStencil(const UINT width, const UINT height, const DXGI_FORMAT resFormat, const UINT arraySize, const UINT mipLevels, const bool isTextureCube)
{
	texture = new Texture(width, height, resFormat, arraySize, mipLevels, true);

	//R32_TYPELESS R16_TYPELESS R32G8X24_TYPELESS R24G8_TYPELESS

	DXGI_FORMAT depthSRVFormat = DXGI_FORMAT_UNKNOWN;
	DXGI_FORMAT stencilSRVFormat = DXGI_FORMAT_UNKNOWN;
	DXGI_FORMAT dsvFormat = DXGI_FORMAT_UNKNOWN;

	switch (resFormat)
	{
	case DXGI_FORMAT_R32_TYPELESS:
		depthSRVFormat = DXGI_FORMAT_R32_FLOAT;
		dsvFormat = DXGI_FORMAT_D32_FLOAT;
		break;
	case DXGI_FORMAT_R16_TYPELESS:
		depthSRVFormat = DXGI_FORMAT_R16_UNORM;
		dsvFormat = DXGI_FORMAT_D16_UNORM;
		break;
	case DXGI_FORMAT_R32G8X24_TYPELESS:
		depthSRVFormat = DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS;
		stencilSRVFormat = DXGI_FORMAT_X32_TYPELESS_G8X24_UINT;
		dsvFormat = DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
		break;
	case DXGI_FORMAT_R24G8_TYPELESS:
		depthSRVFormat = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
		stencilSRVFormat = DXGI_FORMAT_X24_TYPELESS_G8_UINT;
		dsvFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
		break;
	default:
		throw "Unknown format";
		break;
	}

	//create dsvs
	{
		StaticDescriptorHandle descriptorHandle = GlobalDescriptorHeap::getDepthStencilHeap()->allocStaticDescriptor(mipLevels);

		for (UINT i = 0; i < mipLevels; i++)
		{
			if (arraySize > 1)
			{
				D3D12_DEPTH_STENCIL_VIEW_DESC desc = {};
				desc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2DARRAY;
				desc.Format = dsvFormat;
				desc.Texture2DArray.ArraySize = arraySize;
				desc.Texture2DArray.FirstArraySlice = 0;
				desc.Texture2DArray.MipSlice = i;

				GraphicsDevice::get()->CreateDepthStencilView(texture->getResource(), &desc, descriptorHandle.getCPUHandle());

				dsvMipHandles.push_back(descriptorHandle.getCPUHandle());

				descriptorHandle.move();
			}
			else
			{
				D3D12_DEPTH_STENCIL_VIEW_DESC desc = {};
				desc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
				desc.Format = dsvFormat;
				desc.Texture2D.MipSlice = i;

				GraphicsDevice::get()->CreateDepthStencilView(texture->getResource(), &desc, descriptorHandle.getCPUHandle());

				dsvMipHandles.push_back(descriptorHandle.getCPUHandle());

				descriptorHandle.move();
			}
		}
	}

	//create depth and stencil srvs
	{
		//acquire descriptor size = stencil descriptor size + depth descriptor size
		const UINT descriptorNum = static_cast<UINT>((stencilSRVFormat != DXGI_FORMAT_UNKNOWN)) * (1 + mipLevels) + (1 + mipLevels);

		StaticDescriptorHandle descriptorHandle = GlobalDescriptorHeap::getResourceHeap()->allocStaticDescriptor(descriptorNum);

		allDepthIndex = descriptorHandle.getCurrentIndex();

		depthSliceStart = allDepthIndex + 1;

		if (stencilSRVFormat != DXGI_FORMAT_UNKNOWN)
		{
			allStencilIndex = depthSliceStart + mipLevels;
			stencilSliceStart = allStencilIndex + 1;
		}
		else
		{
			allStencilIndex = UINT_MAX;
			stencilSliceStart = UINT_MAX;
		}

		//create depth srvs
		if (isTextureCube)
		{
			const UINT numCube = arraySize / 6;

			if (numCube > 1)
			{
				{
					D3D12_SHADER_RESOURCE_VIEW_DESC desc = {};
					desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
					desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBEARRAY;
					desc.Format = depthSRVFormat;
					desc.TextureCubeArray.First2DArrayFace = 0;
					desc.TextureCubeArray.MipLevels = mipLevels;
					desc.TextureCubeArray.MostDetailedMip = 0;
					desc.TextureCubeArray.NumCubes = numCube;
					desc.TextureCubeArray.ResourceMinLODClamp = 0.f;

					GraphicsDevice::get()->CreateShaderResourceView(texture->getResource(), &desc, descriptorHandle.getCPUHandle());

					descriptorHandle.move();
				}

				for (UINT i = 0; i < mipLevels; i++)
				{
					D3D12_SHADER_RESOURCE_VIEW_DESC desc = {};
					desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
					desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBEARRAY;
					desc.Format = depthSRVFormat;
					desc.TextureCubeArray.First2DArrayFace = 0;
					desc.TextureCubeArray.MipLevels = 1;
					desc.TextureCubeArray.MostDetailedMip = i;
					desc.TextureCubeArray.NumCubes = numCube;
					desc.TextureCubeArray.ResourceMinLODClamp = 0.f;

					GraphicsDevice::get()->CreateShaderResourceView(texture->getResource(), &desc, descriptorHandle.getCPUHandle());

					descriptorHandle.move();
				}
			}
			else
			{
				{
					D3D12_SHADER_RESOURCE_VIEW_DESC desc = {};
					desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
					desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
					desc.Format = depthSRVFormat;
					desc.TextureCube.MipLevels = mipLevels;
					desc.TextureCube.MostDetailedMip = 0;
					desc.TextureCube.ResourceMinLODClamp = 0.f;

					GraphicsDevice::get()->CreateShaderResourceView(texture->getResource(), &desc, descriptorHandle.getCPUHandle());

					descriptorHandle.move();
				}

				for (UINT i = 0; i < mipLevels; i++)
				{
					D3D12_SHADER_RESOURCE_VIEW_DESC desc = {};
					desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
					desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
					desc.Format = depthSRVFormat;
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
			if (arraySize > 1)
			{
				{
					D3D12_SHADER_RESOURCE_VIEW_DESC desc = {};
					desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
					desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DARRAY;
					desc.Format = depthSRVFormat;
					desc.Texture2DArray.ArraySize = arraySize;
					desc.Texture2DArray.FirstArraySlice = 0;
					desc.Texture2DArray.MipLevels = mipLevels;
					desc.Texture2DArray.MostDetailedMip = 0;
					desc.Texture2DArray.PlaneSlice = 0;
					desc.Texture2DArray.ResourceMinLODClamp = 0.f;

					GraphicsDevice::get()->CreateShaderResourceView(texture->getResource(), &desc, descriptorHandle.getCPUHandle());

					descriptorHandle.move();
				}

				for (UINT i = 0; i < mipLevels; i++)
				{
					D3D12_SHADER_RESOURCE_VIEW_DESC desc = {};
					desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
					desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DARRAY;
					desc.Format = depthSRVFormat;
					desc.Texture2DArray.ArraySize = arraySize;
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
				{
					D3D12_SHADER_RESOURCE_VIEW_DESC desc = {};
					desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
					desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
					desc.Format = depthSRVFormat;
					desc.Texture2D.MipLevels = mipLevels;
					desc.Texture2D.MostDetailedMip = 0;
					desc.Texture2D.PlaneSlice = 0;
					desc.Texture2D.ResourceMinLODClamp = 0.f;
					GraphicsDevice::get()->CreateShaderResourceView(texture->getResource(), &desc, descriptorHandle.getCPUHandle());

					descriptorHandle.move();
				}

				for (UINT i = 0; i < mipLevels; i++)
				{
					D3D12_SHADER_RESOURCE_VIEW_DESC desc = {};
					desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
					desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
					desc.Format = depthSRVFormat;
					desc.Texture2D.MipLevels = 1;
					desc.Texture2D.MostDetailedMip = i;
					desc.Texture2D.PlaneSlice = 0;
					desc.Texture2D.ResourceMinLODClamp = 0.f;

					GraphicsDevice::get()->CreateShaderResourceView(texture->getResource(), &desc, descriptorHandle.getCPUHandle());

					descriptorHandle.move();
				}
			}
		}

		if (stencilSRVFormat != DXGI_FORMAT_UNKNOWN)
		{
			//create stencil srvs
			if (isTextureCube)
			{
				const UINT numCube = arraySize / 6;

				if (numCube > 1)
				{
					{
						D3D12_SHADER_RESOURCE_VIEW_DESC desc = {};
						desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
						desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBEARRAY;
						desc.Format = stencilSRVFormat;
						desc.TextureCubeArray.First2DArrayFace = 0;
						desc.TextureCubeArray.MipLevels = mipLevels;
						desc.TextureCubeArray.MostDetailedMip = 0;
						desc.TextureCubeArray.NumCubes = numCube;
						desc.TextureCubeArray.ResourceMinLODClamp = 0.f;

						GraphicsDevice::get()->CreateShaderResourceView(texture->getResource(), &desc, descriptorHandle.getCPUHandle());

						descriptorHandle.move();
					}

					for (UINT i = 0; i < mipLevels; i++)
					{
						D3D12_SHADER_RESOURCE_VIEW_DESC desc = {};
						desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
						desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBEARRAY;
						desc.Format = stencilSRVFormat;
						desc.TextureCubeArray.First2DArrayFace = 0;
						desc.TextureCubeArray.MipLevels = 1;
						desc.TextureCubeArray.MostDetailedMip = i;
						desc.TextureCubeArray.NumCubes = numCube;
						desc.TextureCubeArray.ResourceMinLODClamp = 0.f;

						GraphicsDevice::get()->CreateShaderResourceView(texture->getResource(), &desc, descriptorHandle.getCPUHandle());

						descriptorHandle.move();
					}
				}
				else
				{
					{
						D3D12_SHADER_RESOURCE_VIEW_DESC desc = {};
						desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
						desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
						desc.Format = stencilSRVFormat;
						desc.TextureCube.MipLevels = mipLevels;
						desc.TextureCube.MostDetailedMip = 0;
						desc.TextureCube.ResourceMinLODClamp = 0.f;

						GraphicsDevice::get()->CreateShaderResourceView(texture->getResource(), &desc, descriptorHandle.getCPUHandle());

						descriptorHandle.move();
					}

					for (UINT i = 0; i < mipLevels; i++)
					{
						D3D12_SHADER_RESOURCE_VIEW_DESC desc = {};
						desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
						desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
						desc.Format = stencilSRVFormat;
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
				if (arraySize > 1)
				{
					{
						D3D12_SHADER_RESOURCE_VIEW_DESC desc = {};
						desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
						desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DARRAY;
						desc.Format = stencilSRVFormat;
						desc.Texture2DArray.ArraySize = arraySize;
						desc.Texture2DArray.FirstArraySlice = 0;
						desc.Texture2DArray.MipLevels = mipLevels;
						desc.Texture2DArray.MostDetailedMip = 0;
						desc.Texture2DArray.PlaneSlice = 0;
						desc.Texture2DArray.ResourceMinLODClamp = 0.f;

						GraphicsDevice::get()->CreateShaderResourceView(texture->getResource(), &desc, descriptorHandle.getCPUHandle());

						descriptorHandle.move();
					}

					for (UINT i = 0; i < mipLevels; i++)
					{
						D3D12_SHADER_RESOURCE_VIEW_DESC desc = {};
						desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
						desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DARRAY;
						desc.Format = stencilSRVFormat;
						desc.Texture2DArray.ArraySize = arraySize;
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
					{
						D3D12_SHADER_RESOURCE_VIEW_DESC desc = {};
						desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
						desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
						desc.Format = stencilSRVFormat;
						desc.Texture2D.MipLevels = mipLevels;
						desc.Texture2D.MostDetailedMip = 0;
						desc.Texture2D.PlaneSlice = 0;
						desc.Texture2D.ResourceMinLODClamp = 0.f;
						GraphicsDevice::get()->CreateShaderResourceView(texture->getResource(), &desc, descriptorHandle.getCPUHandle());

						descriptorHandle.move();
					}

					for (UINT i = 0; i < mipLevels; i++)
					{
						D3D12_SHADER_RESOURCE_VIEW_DESC desc = {};
						desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
						desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
						desc.Format = stencilSRVFormat;
						desc.Texture2D.MipLevels = 1;
						desc.Texture2D.MostDetailedMip = i;
						desc.Texture2D.PlaneSlice = 0;
						desc.Texture2D.ResourceMinLODClamp = 0.f;

						GraphicsDevice::get()->CreateShaderResourceView(texture->getResource(), &desc, descriptorHandle.getCPUHandle());

						descriptorHandle.move();
					}
				}
			}
		}
	}
}

TextureDepthStencil::TextureDepthStencil(const TextureDepthStencil& tds) :
	allDepthIndex(tds.allDepthIndex),
	allStencilIndex(tds.allStencilIndex),
	depthSliceStart(tds.depthSliceStart),
	stencilSliceStart(tds.stencilSliceStart),
	dsvMipHandles(tds.dsvMipHandles),
	texture(new Texture(*(tds.texture)))
{
}

TextureDepthStencil::~TextureDepthStencil()
{
	if (texture)
	{
		delete texture;
	}
}

ShaderResourceDesc TextureDepthStencil::getAllDepthIndex() const
{
	ShaderResourceDesc desc = {};
	desc.type = ShaderResourceDesc::TEXTURE;
	desc.state = ShaderResourceDesc::SRV;
	desc.textureDesc.texture = texture;
	desc.textureDesc.mipSlice = D3D12_TRANSITION_ALL_MIPLEVELS;
	desc.textureDesc.resourceIndex = allDepthIndex;

	return desc;
}

ShaderResourceDesc TextureDepthStencil::getAllStencilIndex() const
{
	ShaderResourceDesc desc = {};
	desc.type = ShaderResourceDesc::TEXTURE;
	desc.state = ShaderResourceDesc::SRV;
	desc.textureDesc.texture = texture;
	desc.textureDesc.mipSlice = D3D12_TRANSITION_ALL_MIPLEVELS;
	desc.textureDesc.resourceIndex = allStencilIndex;

	return desc;
}

ShaderResourceDesc TextureDepthStencil::getDepthMipIndex(const UINT mipSlice) const
{
	ShaderResourceDesc desc = {};
	desc.type = ShaderResourceDesc::TEXTURE;
	desc.state = ShaderResourceDesc::SRV;
	desc.textureDesc.texture = texture;
	desc.textureDesc.mipSlice = mipSlice;
	desc.textureDesc.resourceIndex = depthSliceStart + mipSlice;

	return desc;
}

ShaderResourceDesc TextureDepthStencil::getStencilMipIndex(const UINT mipSlice) const
{
	ShaderResourceDesc desc = {};
	desc.type = ShaderResourceDesc::TEXTURE;
	desc.state = ShaderResourceDesc::SRV;
	desc.textureDesc.texture = texture;
	desc.textureDesc.mipSlice = mipSlice;
	desc.textureDesc.resourceIndex = stencilSliceStart + mipSlice;

	return desc;
}

DepthStencilDesc TextureDepthStencil::getDSVMipHandle(const UINT mipSlice) const
{
	DepthStencilDesc desc = {};
	desc.texture = texture;
	desc.mipSlice = mipSlice;
	desc.dsvHandle = dsvMipHandles[mipSlice];

	return desc;
}

Texture* TextureDepthStencil::getTexture() const
{
	return texture;
}
