#pragma once

#ifndef _GEAR_CORE_RESOURCE_TEXTURERENDERVIEW_H_
#define _GEAR_CORE_RESOURCE_TEXTURERENDERVIEW_H_

#include"D3D12Resource/Texture.h"

#include"ResourceBase.h"

namespace Gear
{
	namespace Core
	{
		namespace Resource
		{
			//多用途纹理
			//texturecubearray
			//texturecube
			//texture2darray
			//texture2d
			//SRV/RTV/UAV
			class TextureRenderView :public ResourceBase
			{
			public:

				TextureRenderView() = delete;

				TextureRenderView(D3D12Resource::Texture* const texture, const bool isTextureCube, const bool persistent, const DXGI_FORMAT srvFormat, const DXGI_FORMAT uavFormat, const DXGI_FORMAT rtvFormat);

				TextureRenderView(const TextureRenderView&);

				~TextureRenderView();

				D3D12Resource::ShaderResourceDesc getAllSRVIndex() const;

				D3D12Resource::ShaderResourceDesc getSRVMipIndex(const uint32_t mipSlice) const;

				D3D12Resource::ShaderResourceDesc getUAVMipIndex(const uint32_t mipSlice) const;

				D3D12Resource::RenderTargetDesc getRTVMipHandle(const uint32_t mipSlice) const;

				D3D12Resource::ClearUAVDesc getClearUAVDesc(const uint32_t mipSlice) const;

				D3D12Resource::Texture* getTexture() const;

				void copyDescriptors() override;

				const bool hasRTV;

				const bool hasUAV;

			private:

				uint32_t allSRVIndex;

				uint32_t srvMipIndexStart;

				uint32_t uavMipIndexStart;

				D3D12_CPU_DESCRIPTOR_HANDLE rtvMipHandleStart;

				D3D12_GPU_DESCRIPTOR_HANDLE viewGPUHandleStart;

				D3D12_CPU_DESCRIPTOR_HANDLE viewCPUHandleStart;

				D3D12Resource::Texture* texture;
			};
		}
	}
}

#endif // !_GEAR_CORE_RESOURCE_TEXTURERENDERVIEW_H_