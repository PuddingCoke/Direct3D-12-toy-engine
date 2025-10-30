#include<Gear/Core/Resource/TextureRenderView.h>

Gear::Core::Resource::TextureRenderView::TextureRenderView(D3D12Resource::Texture* const texture, const bool isTextureCube, const bool persistent, const DXGI_FORMAT srvFormat, const DXGI_FORMAT uavFormat, const DXGI_FORMAT rtvFormat) :
	ResourceBase(persistent), texture(texture), hasRTV((rtvFormat != DXGI_FORMAT_UNKNOWN)), hasUAV((uavFormat != DXGI_FORMAT_UNKNOWN)), rtvMipHandleStart(), viewGPUHandleStart(), viewCPUHandleStart()
{
	//创建SRV、UAV
	{
		numSRVUAVCBVDescriptors = 1 + texture->getMipLevels() + static_cast<uint32_t>(hasUAV) * texture->getMipLevels();

		D3D12Core::DescriptorHandle descriptorHandle;

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

		if (isTextureCube) //立方体纹理的SRV创建
		{
			const uint32_t cubeNum = texture->getArraySize() / 6;

			if (cubeNum > 1)
			{
				//创建能访问所有mipslice的SRV
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

				//创建访问每个mipslice的SRV
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

					GraphicsDevice::get()->CreateShaderResourceView(texture->getResource(), &desc, descriptorHandle.getCPUHandle());

					descriptorHandle.move();
				}
			}
			else
			{
				//创建能访问所有mipslice的SRV
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

				//创建访问每个mipslice的SRV
				for (uint32_t i = 0; i < texture->getMipLevels(); i++)
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
				//创建能访问所有mipslice的SRV
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

				//创建访问每个mipslice的SRV
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

					GraphicsDevice::get()->CreateShaderResourceView(texture->getResource(), &desc, descriptorHandle.getCPUHandle());

					descriptorHandle.move();
				}
			}
			else
			{
				//创建能访问所有mipslice的SRV
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

				//创建访问每个mipslice的SRV
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

					GraphicsDevice::get()->CreateShaderResourceView(texture->getResource(), &desc, descriptorHandle.getCPUHandle());

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

				nonShaderVisibleHandle = GlobalDescriptorHeap::getNonShaderVisibleResourceHeap()->allocStaticDescriptor(texture->getMipLevels());

				viewCPUHandleStart = nonShaderVisibleHandle.getCPUHandle();
			}
			else
			{
				viewCPUHandleStart = descriptorHandle.getCPUHandle();

				//之后获取viewGPUHandleStart
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

					GraphicsDevice::get()->CreateUnorderedAccessView(texture->getResource(), nullptr, &desc, descriptorHandle.getCPUHandle());

					descriptorHandle.move();

					if (persistent)
					{
						//同一时间在非着色器可见的资源描述符堆创建UAV
						GraphicsDevice::get()->CreateUnorderedAccessView(texture->getResource(), nullptr, &desc, nonShaderVisibleHandle.getCPUHandle());

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

					GraphicsDevice::get()->CreateUnorderedAccessView(texture->getResource(), nullptr, &desc, descriptorHandle.getCPUHandle());

					descriptorHandle.move();

					if (persistent)
					{
						//同一时间在非着色器可见的资源描述符堆创建UAV
						GraphicsDevice::get()->CreateUnorderedAccessView(texture->getResource(), nullptr, &desc, nonShaderVisibleHandle.getCPUHandle());

						nonShaderVisibleHandle.move();
					}
				}
			}
		}
	}

	//创建RTV
	if (hasRTV)
	{
		D3D12Core::DescriptorHandle descriptorHandle;

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
			for (uint32_t i = 0; i < texture->getMipLevels(); i++)
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
			for (uint32_t i = 0; i < texture->getMipLevels(); i++)
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

Gear::Core::Resource::TextureRenderView::TextureRenderView(const TextureRenderView& trv) :
	ResourceBase(trv.persistent),
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

Gear::Core::Resource::TextureRenderView::~TextureRenderView()
{
	if (texture)
	{
		delete texture;
	}
}

Gear::Core::Resource::D3D12Resource::ShaderResourceDesc Gear::Core::Resource::TextureRenderView::getAllSRVIndex() const
{
	D3D12Resource::ShaderResourceDesc desc = {};
	desc.type = D3D12Resource::ShaderResourceDesc::TEXTURE;
	desc.state = D3D12Resource::ShaderResourceDesc::SRV;
	desc.resourceIndex = allSRVIndex;
	desc.textureDesc.texture = texture;
	desc.textureDesc.mipSlice = D3D12Resource::D3D12_TRANSITION_ALL_MIPLEVELS;

	return desc;
}

Gear::Core::Resource::D3D12Resource::ShaderResourceDesc Gear::Core::Resource::TextureRenderView::getSRVMipIndex(const uint32_t mipSlice) const
{
	D3D12Resource::ShaderResourceDesc desc = {};
	desc.type = D3D12Resource::ShaderResourceDesc::TEXTURE;
	desc.state = D3D12Resource::ShaderResourceDesc::SRV;
	desc.resourceIndex = srvMipIndexStart + mipSlice;
	desc.textureDesc.texture = texture;
	desc.textureDesc.mipSlice = mipSlice;

	return desc;
}

Gear::Core::Resource::D3D12Resource::ShaderResourceDesc Gear::Core::Resource::TextureRenderView::getUAVMipIndex(const uint32_t mipSlice) const
{
	D3D12Resource::ShaderResourceDesc desc = {};
	desc.type = D3D12Resource::ShaderResourceDesc::TEXTURE;
	desc.state = D3D12Resource::ShaderResourceDesc::UAV;
	desc.resourceIndex = uavMipIndexStart + mipSlice;
	desc.textureDesc.texture = texture;
	desc.textureDesc.mipSlice = mipSlice;

	return desc;
}

Gear::Core::Resource::D3D12Resource::RenderTargetDesc Gear::Core::Resource::TextureRenderView::getRTVMipHandle(const uint32_t mipSlice) const
{
	D3D12Resource::RenderTargetDesc desc = {};
	desc.texture = texture;
	desc.mipSlice = mipSlice;
	desc.rtvHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(rtvMipHandleStart, mipSlice, GlobalDescriptorHeap::getRenderTargetIncrementSize());

	return desc;
}

Gear::Core::Resource::D3D12Resource::ClearUAVDesc Gear::Core::Resource::TextureRenderView::getClearUAVDesc(const uint32_t mipSlice) const
{
	D3D12Resource::ClearUAVDesc desc = {};
	desc.type = D3D12Resource::ClearUAVDesc::TEXTURE;
	desc.textureDesc.texture = texture;
	desc.textureDesc.mipSlice = mipSlice;
	desc.viewGPUHandle = CD3DX12_GPU_DESCRIPTOR_HANDLE(viewGPUHandleStart, mipSlice, GlobalDescriptorHeap::getResourceIncrementSize());
	desc.viewCPUHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(viewCPUHandleStart, mipSlice, GlobalDescriptorHeap::getResourceIncrementSize());

	return desc;
}

Gear::Core::Resource::D3D12Resource::Texture* Gear::Core::Resource::TextureRenderView::getTexture() const
{
	return texture;
}

void Gear::Core::Resource::TextureRenderView::copyDescriptors()
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
