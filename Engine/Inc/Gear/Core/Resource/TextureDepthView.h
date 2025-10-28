#pragma once

#ifndef _CORE_RESOURCE_TEXTUREDEPTHVIEW_H_
#define _CORE_RESOURCE_TEXTUREDEPTHVIEW_H_

#include"D3D12Resource/Texture.h"

#include"EngineResource.h"

namespace Core
{
	namespace Resource
	{
		class TextureDepthView :public EngineResource
		{
		public:

			TextureDepthView() = delete;

			//supported types
			//R32_TYPELESS 
			//R16_TYPELESS 
			//R32G8X24_TYPELESS 
			//R24G8_TYPELESS
			TextureDepthView(D3D12Resource::Texture* const texture, const bool isTextureCube, const bool persistent);

			TextureDepthView(const TextureDepthView&);

			~TextureDepthView();

			D3D12Resource::ShaderResourceDesc getAllDepthIndex() const;

			D3D12Resource::ShaderResourceDesc getAllStencilIndex() const;

			D3D12Resource::ShaderResourceDesc getDepthMipIndex(const uint32_t mipSlice) const;

			D3D12Resource::ShaderResourceDesc getStencilMipIndex(const uint32_t mipSlice) const;

			D3D12Resource::DepthStencilDesc getDSVMipHandle(const uint32_t mipSlice) const;

			D3D12Resource::Texture* getTexture() const;

			void copyDescriptors() override;

		private:

			uint32_t allDepthIndex;

			uint32_t allStencilIndex;

			uint32_t depthMipIndexStart;

			uint32_t stencilMipIndexStart;

			D3D12_CPU_DESCRIPTOR_HANDLE dsvMipHandleStart;

			D3D12Resource::Texture* texture;
		};
	}
}

#endif // !_CORE_RESOURCE_TEXTUREDEPTHVIEW_H_