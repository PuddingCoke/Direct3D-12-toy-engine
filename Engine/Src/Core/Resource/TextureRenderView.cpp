#include<Gear/Core/Resource/TextureRenderView.h>

Core::Resource::TextureRenderView::TextureRenderView(D3D12Resource::Texture* const texture, const bool isTextureCube, const bool persistent, const DXGI_FORMAT srvFormat, const DXGI_FORMAT uavFormat, const DXGI_FORMAT rtvFormat) :
	EngineResource(persistent), texture(texture), hasRTV((rtvFormat != DXGI_FORMAT_UNKNOWN)), hasUAV((uavFormat != DXGI_FORMAT_UNKNOWN)), rtvMipHandleStart(), viewGPUHandleStart(), viewCPUHandleStart()
{
	//create srv uav descriptors
	{
		//plus 1 because there is a additional srv to acess all mipslices
		numSRVUAVCBVDescriptors = 1 + texture->getMipLevels() + static_cast<uint32_t>(hasUAV) * texture->getMipLevels();

		D3D12Core::DescriptorHandle descriptorHandle;

		if (persistent)
		{
			descriptorHandle = Core::GlobalDescriptorHeap::getResourceHeap()->allocStaticDescriptor(numSRVUAVCBVDescriptors);
		}
		else
		{
			descriptorHandle = Core::GlobalDescriptorHeap::getNonShaderVisibleResourceHeap()->allocDynamicDescriptor(numSRVUAVCBVDescriptors);
		}

		srvUAVCBVHandleStart = descriptorHandle.getCPUHandle();

		allSRVIndex = descriptorHandle.getCurrentIndex();

		srvMipIndexStart = allSRVIndex + 1;

		uavMipIndexStart = srvMipIndexStart + texture->getMipLevels();

		if (isTextureCube) //TextureCube srv creation
		{
			const uint32_t cubeNum = texture->getArraySize() / 6;

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

					Core::GraphicsDevice::get()->CreateShaderResourceView(texture->getResource(), &desc, descriptorHandle.getCPUHandle());

					descriptorHandle.move();
				}

				//create srv acess each mipSlice
				for (uint32_t i = 0; i < texture->getMipLevels(); i++)
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

					Core::GraphicsDevice::get()->CreateShaderResourceView(texture->getResource(), &desc, descriptorHandle.getCPUHandle());

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

					Core::GraphicsDevice::get()->CreateShaderResourceView(texture->getResource(), &desc, descriptorHandle.getCPUHandle());

					descriptorHandle.move();
				}

				//create srv acess each mipSlice
				for (uint32_t i = 0; i < texture->getMipLevels(); i++)
				{
					D3D12_SHADER_RESOURCE_VIEW_DESC desc = {};
					desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
					desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
					desc.Format = srvFormat;
					desc.TextureCube.MipLevels = 1;
					desc.TextureCube.MostDetailedMip = i;
					desc.TextureCube.ResourceMinLODClamp = 0.f;

					Core::GraphicsDevice::get()->CreateShaderResourceView(texture->getResource(), &desc, descriptorHandle.getCPUHandle());

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

					Core::GraphicsDevice::get()->CreateShaderResourceView(texture->getResource(), &desc, descriptorHandle.getCPUHandle());

					descriptorHandle.move();
				}

				//create srv acess each mipSlice
				for (uint32_t i = 0; i < texture->getMipLevels(); i++)
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

					Core::GraphicsDevice::get()->CreateShaderResourceView(texture->getResource(), &desc, descriptorHandle.getCPUHandle());

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

					Core::GraphicsDevice::get()->CreateShaderResourceView(texture->getResource(), &desc, descriptorHandle.getCPUHandle());

					descriptorHandle.move();
				}

				//create srv acess each mipSlice
				for (uint32_t i = 0; i < texture->getMipLevels(); i++)
				{
					D3D12_SHADER_RESOURCE_VIEW_DESC desc = {};
					desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
					desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
					desc.Format = srvFormat;
					desc.Texture2D.MipLevels = 1;
					desc.Texture2D.MostDetailedMip = i;
					desc.Texture2D.PlaneSlice = 0;
					desc.Texture2D.ResourceMinLODClamp = 0.f;

					Core::GraphicsDevice::get()->CreateShaderResourceView(texture->getResource(), &desc, descriptorHandle.getCPUHandle());

					descriptorHandle.move();
				}
			}
		}

		if (hasUAV)
		{
			D3D12Core::DescriptorHandle nonShaderVisibleHandle;

			if (persistent)
			{
				viewGPUHandleStart = descriptorHandle.getGPUHandle();

				nonShaderVisibleHandle = Core::GlobalDescriptorHeap::getNonShaderVisibleResourceHeap()->allocStaticDescriptor(texture->getMipLevels());

				viewCPUHandleStart = nonShaderVisibleHandle.getCPUHandle();
			}
			else
			{
				viewCPUHandleStart = descriptorHandle.getCPUHandle();

				//get viewGPUHandleStart later
			}

			if (texture->getArraySize() > 1)
			{
				for (uint32_t i = 0; i < texture->getMipLevels(); i++)
				{
					D3D12_UNORDERED_ACCESS_VIEW_DESC desc = {};
					desc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2DARRAY;
					desc.Format = uavFormat;
					desc.Texture2DArray.ArraySize = texture->getArraySize();
					desc.Texture2DArray.FirstArraySlice = 0;
					desc.Texture2DArray.MipSlice = i;
					desc.Texture2DArray.PlaneSlice = 0;

					Core::GraphicsDevice::get()->CreateUnorderedAccessView(texture->getResource(), nullptr, &desc, descriptorHandle.getCPUHandle());

					descriptorHandle.move();

					if (persistent)
					{
						//create uav to non shader visible heap at the same time
						Core::GraphicsDevice::get()->CreateUnorderedAccessView(texture->getResource(), nullptr, &desc, nonShaderVisibleHandle.getCPUHandle());

						nonShaderVisibleHandle.move();
					}
				}
			}
			else
			{
				for (uint32_t i = 0; i < texture->getMipLevels(); i++)
				{
					D3D12_UNORDERED_ACCESS_VIEW_DESC desc = {};
					desc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
					desc.Format = uavFormat;
					desc.Texture2D.MipSlice = i;
					desc.Texture2D.PlaneSlice = 0;

					Core::GraphicsDevice::get()->CreateUnorderedAccessView(texture->getResource(), nullptr, &desc, descriptorHandle.getCPUHandle());

					descriptorHandle.move();

					if (persistent)
					{
						//create uav to non shader visible heap at the same time
						Core::GraphicsDevice::get()->CreateUnorderedAccessView(texture->getResource(), nullptr, &desc, nonShaderVisibleHandle.getCPUHandle());

						nonShaderVisibleHandle.move();
					}
				}
			}
		}
	}

	//create rtv descriptor
	if (hasRTV)
	{
		D3D12Core::DescriptorHandle descriptorHandle;

		if (persistent)
		{
			descriptorHandle = Core::GlobalDescriptorHeap::getRenderTargetHeap()->allocStaticDescriptor(texture->getMipLevels());
		}
		else
		{
			descriptorHandle = Core::GlobalDescriptorHeap::getRenderTargetHeap()->allocDynamicDescriptor(texture->getMipLevels());
		}

		rtvMipHandleStart = descriptorHandle.getCPUHandle();

		if (texture->getArraySize() > 1)
		{
			for (uint32_t i = 0; i < texture->getMipLevels(); i++)
			{
				D3D12_RENDER_TARGET_VIEW_DESC desc = {};
				desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DARRAY;
				desc.Format = rtvFormat;
				desc.Texture2DArray.ArraySize = texture->getArraySize();
				desc.Texture2DArray.FirstArraySlice = 0;
				desc.Texture2DArray.MipSlice = i;
				desc.Texture2DArray.PlaneSlice = 0;

				Core::GraphicsDevice::get()->CreateRenderTargetView(texture->getResource(), &desc, descriptorHandle.getCPUHandle());

				descriptorHandle.move();
			}
		}
		else
		{
			for (uint32_t i = 0; i < texture->getMipLevels(); i++)
			{
				D3D12_RENDER_TARGET_VIEW_DESC desc = {};
				desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
				desc.Format = rtvFormat;
				desc.Texture2D.MipSlice = i;
				desc.Texture2D.PlaneSlice = 0;

				Core::GraphicsDevice::get()->CreateRenderTargetView(texture->getResource(), &desc, descriptorHandle.getCPUHandle());

				descriptorHandle.move();
			}
		}
	}
}

Core::Resource::TextureRenderView::TextureRenderView(const TextureRenderView& trv) :
	EngineResource(trv.persistent),
	hasRTV(trv.hasRTV),
	hasUAV(trv.hasUAV),
	allSRVIndex(trv.allSRVIndex),
	srvMipIndexStart(trv.srvMipIndexStart),
	uavMipIndexStart(trv.uavMipIndexStart),
	rtvMipHandleStart(trv.rtvMipHandleStart),
	viewGPUHandleStart(trv.viewGPUHandleStart),
	viewCPUHandleStart(trv.viewCPUHandleStart),
	texture(new D3D12Resource::Texture(trv.texture))
{
}

Core::Resource::TextureRenderView::~TextureRenderView()
{
	if (texture)
	{
		delete texture;
	}
}

Core::Resource::D3D12Resource::ShaderResourceDesc Core::Resource::TextureRenderView::getAllSRVIndex() const
{
	D3D12Resource::ShaderResourceDesc desc = {};
	desc.type = D3D12Resource::ShaderResourceDesc::TEXTURE;
	desc.state = D3D12Resource::ShaderResourceDesc::SRV;
	desc.resourceIndex = allSRVIndex;
	desc.textureDesc.texture = texture;
	desc.textureDesc.mipSlice = D3D12Resource::D3D12_TRANSITION_ALL_MIPLEVELS;

	return desc;
}

Core::Resource::D3D12Resource::ShaderResourceDesc Core::Resource::TextureRenderView::getSRVMipIndex(const uint32_t mipSlice) const
{
	D3D12Resource::ShaderResourceDesc desc = {};
	desc.type = D3D12Resource::ShaderResourceDesc::TEXTURE;
	desc.state = D3D12Resource::ShaderResourceDesc::SRV;
	desc.resourceIndex = srvMipIndexStart + mipSlice;
	desc.textureDesc.texture = texture;
	desc.textureDesc.mipSlice = mipSlice;

	return desc;
}

Core::Resource::D3D12Resource::ShaderResourceDesc Core::Resource::TextureRenderView::getUAVMipIndex(const uint32_t mipSlice) const
{
	D3D12Resource::ShaderResourceDesc desc = {};
	desc.type = D3D12Resource::ShaderResourceDesc::TEXTURE;
	desc.state = D3D12Resource::ShaderResourceDesc::UAV;
	desc.resourceIndex = uavMipIndexStart + mipSlice;
	desc.textureDesc.texture = texture;
	desc.textureDesc.mipSlice = mipSlice;

	return desc;
}

Core::Resource::D3D12Resource::RenderTargetDesc Core::Resource::TextureRenderView::getRTVMipHandle(const uint32_t mipSlice) const
{
	D3D12Resource::RenderTargetDesc desc = {};
	desc.texture = texture;
	desc.mipSlice = mipSlice;
	desc.rtvHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(rtvMipHandleStart, mipSlice, Core::GlobalDescriptorHeap::getRenderTargetIncrementSize());

	return desc;
}

Core::Resource::D3D12Resource::ClearUAVDesc Core::Resource::TextureRenderView::getClearUAVDesc(const uint32_t mipSlice) const
{
	D3D12Resource::ClearUAVDesc desc = {};
	desc.type = D3D12Resource::ClearUAVDesc::TEXTURE;
	desc.textureDesc.texture = texture;
	desc.textureDesc.mipSlice = mipSlice;
	desc.viewGPUHandle = CD3DX12_GPU_DESCRIPTOR_HANDLE(viewGPUHandleStart, mipSlice, Core::GlobalDescriptorHeap::getResourceIncrementSize());
	desc.viewCPUHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(viewCPUHandleStart, mipSlice, Core::GlobalDescriptorHeap::getResourceIncrementSize());

	return desc;
}

Core::Resource::D3D12Resource::Texture* Core::Resource::TextureRenderView::getTexture() const
{
	return texture;
}

void Core::Resource::TextureRenderView::copyDescriptors()
{
	D3D12Core::DescriptorHandle shaderVisibleHandle = getTransientDescriptorHandle();

	allSRVIndex = shaderVisibleHandle.getCurrentIndex();

	srvMipIndexStart = allSRVIndex + 1;

	if (hasUAV)
	{
		uavMipIndexStart = allSRVIndex + 1 + texture->getMipLevels();

		shaderVisibleHandle.offset(1 + texture->getMipLevels());

		viewGPUHandleStart = shaderVisibleHandle.getGPUHandle();
	}
}
