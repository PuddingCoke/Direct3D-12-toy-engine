#include<Gear/Core/Resource/TextureRenderView.h>

TextureRenderView::TextureRenderView(Texture* const texture, const bool isTextureCube, const bool persistent, const DXGI_FORMAT srvFormat, const DXGI_FORMAT uavFormat, const DXGI_FORMAT rtvFormat) :
	texture(texture), hasRTV((rtvFormat != DXGI_FORMAT_UNKNOWN)), hasUAV((uavFormat != DXGI_FORMAT_UNKNOWN)), rtvMipHandleStart(), viewGPUHandleStart(), viewCPUHandleStart()
{
	//create srv uav descriptors
	{
		//plus 1 because there is a additional srv to acess all mipslices
		numSRVUAVCBVDescriptors = 1 + texture->getMipLevels() + static_cast<UINT>(hasUAV) * texture->getMipLevels();

		DescriptorHandle descriptorHandle = DescriptorHandle();

		if (persistent)
		{
			descriptorHandle = GlobalDescriptorHeap::getResourceHeap()->allocStaticDescriptor(numSRVUAVCBVDescriptors);
		}
		else
		{
			descriptorHandle = GlobalDescriptorHeap::getNonShaderVisibleResourceHeap()->allocDynamicDescriptor(numSRVUAVCBVDescriptors);
		}

		srvUAVCBVHandleStart = descriptorHandle.getCPUHandle();

		allSRVIndex = descriptorHandle.getCurrentIndex();

		srvMipIndexStart = allSRVIndex + 1;

		uavMipIndexStart = srvMipIndexStart + texture->getMipLevels();

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
			DescriptorHandle nonShaderVisibleHandle;

			if (persistent)
			{
				viewGPUHandleStart = descriptorHandle.getGPUHandle();

				nonShaderVisibleHandle = GlobalDescriptorHeap::getNonShaderVisibleResourceHeap()->allocStaticDescriptor(texture->getMipLevels());

				viewCPUHandleStart = nonShaderVisibleHandle.getCPUHandle();
			}
			else
			{
				viewCPUHandleStart = descriptorHandle.getCPUHandle();

				//get viewGPUHandleStart later
			}

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

					if (persistent)
					{
						GraphicsDevice::get()->CreateUnorderedAccessView(texture->getResource(), nullptr, &desc, nonShaderVisibleHandle.getCPUHandle());

						nonShaderVisibleHandle.move();
					}
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

					if (persistent)
					{
						GraphicsDevice::get()->CreateUnorderedAccessView(texture->getResource(), nullptr, &desc, nonShaderVisibleHandle.getCPUHandle());

						nonShaderVisibleHandle.move();
					}
				}
			}
		}
	}

	//create rtv descriptor
	if (hasRTV)
	{
		DescriptorHandle descriptorHandle = DescriptorHandle();

		if (persistent)
		{
			descriptorHandle = GlobalDescriptorHeap::getRenderTargetHeap()->allocStaticDescriptor(texture->getMipLevels());
		}
		else
		{
			descriptorHandle = GlobalDescriptorHeap::getRenderTargetHeap()->allocDynamicDescriptor(texture->getMipLevels());
		}

		rtvMipHandleStart = descriptorHandle.getCPUHandle();

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

				descriptorHandle.move();
			}
		}
	}
}

TextureRenderView::TextureRenderView(const TextureRenderView& trv) :
	hasRTV(trv.hasRTV),
	hasUAV(trv.hasUAV),
	allSRVIndex(trv.allSRVIndex),
	srvMipIndexStart(trv.srvMipIndexStart),
	uavMipIndexStart(trv.uavMipIndexStart),
	rtvMipHandleStart(trv.rtvMipHandleStart),
	texture(new Texture(*(trv.texture)))
{
}

TextureRenderView::~TextureRenderView()
{
	if (texture)
	{
		delete texture;
	}
}

ShaderResourceDesc TextureRenderView::getAllSRVIndex() const
{
	ShaderResourceDesc desc = {};
	desc.type = ShaderResourceDesc::TEXTURE;
	desc.state = ShaderResourceDesc::SRV;
	desc.resourceIndex = allSRVIndex;
	desc.textureDesc.texture = texture;
	desc.textureDesc.mipSlice = D3D12_TRANSITION_ALL_MIPLEVELS;

	return desc;
}

ShaderResourceDesc TextureRenderView::getSRVMipIndex(const UINT mipSlice) const
{
	ShaderResourceDesc desc = {};
	desc.type = ShaderResourceDesc::TEXTURE;
	desc.state = ShaderResourceDesc::SRV;
	desc.resourceIndex = srvMipIndexStart + mipSlice;
	desc.textureDesc.texture = texture;
	desc.textureDesc.mipSlice = mipSlice;

	return desc;
}

ShaderResourceDesc TextureRenderView::getUAVMipIndex(const UINT mipSlice) const
{
	ShaderResourceDesc desc = {};
	desc.type = ShaderResourceDesc::TEXTURE;
	desc.state = ShaderResourceDesc::UAV;
	desc.resourceIndex = uavMipIndexStart + mipSlice;
	desc.textureDesc.texture = texture;
	desc.textureDesc.mipSlice = mipSlice;

	return desc;
}

RenderTargetDesc TextureRenderView::getRTVMipHandle(const UINT mipSlice) const
{
	RenderTargetDesc desc = {};
	desc.texture = texture;
	desc.mipSlice = mipSlice;
	desc.rtvHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(rtvMipHandleStart, mipSlice, GlobalDescriptorHeap::getRenderTargetIncrementSize());

	return desc;
}

ClearUAVDesc TextureRenderView::getClearUAVDesc(const UINT mipSlice) const
{
	ClearUAVDesc desc = {};
	desc.type = ClearUAVDesc::TEXTURE;
	desc.textureDesc.texture = texture;
	desc.textureDesc.mipSlice = mipSlice;
	desc.viewGPUHandle = CD3DX12_GPU_DESCRIPTOR_HANDLE(viewGPUHandleStart, mipSlice, GlobalDescriptorHeap::getResourceIncrementSize());
	desc.viewCPUHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(viewCPUHandleStart, mipSlice, GlobalDescriptorHeap::getResourceIncrementSize());

	return desc;
}

Texture* TextureRenderView::getTexture() const
{
	return texture;
}

void TextureRenderView::copyDescriptors()
{
	DescriptorHandle shaderVisibleHandle = GlobalDescriptorHeap::getResourceHeap()->allocDynamicDescriptor(numSRVUAVCBVDescriptors);

	GraphicsDevice::get()->CopyDescriptorsSimple(numSRVUAVCBVDescriptors, shaderVisibleHandle.getCPUHandle(), srvUAVCBVHandleStart, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	allSRVIndex = shaderVisibleHandle.getCurrentIndex();

	srvMipIndexStart = allSRVIndex + 1;

	if (hasUAV)
	{
		uavMipIndexStart = allSRVIndex + 1 + texture->getMipLevels();

		shaderVisibleHandle.offset(1 + texture->getMipLevels());

		viewGPUHandleStart = shaderVisibleHandle.getGPUHandle();
	}
}
