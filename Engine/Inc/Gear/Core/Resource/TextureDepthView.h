#pragma once

#ifndef _TEXTUREDEPTHVIEW_H_
#define _TEXTUREDEPTHVIEW_H_

#include<Gear/Core/DX/Texture.h>

#include"EngineResource.h"

class TextureDepthView :public EngineResource
{
public:

	//supported types
	//R32_TYPELESS 
	//R16_TYPELESS 
	//R32G8X24_TYPELESS 
	//R24G8_TYPELESS
	TextureDepthView(Texture* const texture, const bool isTextureCube, const bool persistent);

	TextureDepthView(const TextureDepthView&);

	~TextureDepthView();

	ShaderResourceDesc getAllDepthIndex() const;

	ShaderResourceDesc getAllStencilIndex() const;

	ShaderResourceDesc getDepthMipIndex(const uint32_t mipSlice) const;

	ShaderResourceDesc getStencilMipIndex(const uint32_t mipSlice) const;

	DepthStencilDesc getDSVMipHandle(const uint32_t mipSlice) const;

	Texture* getTexture() const;

	void copyDescriptors() override;

private:

	uint32_t allDepthIndex;

	uint32_t allStencilIndex;

	uint32_t depthMipIndexStart;

	uint32_t stencilMipIndexStart;

	D3D12_CPU_DESCRIPTOR_HANDLE dsvMipHandleStart;

	Texture* texture;
};

#endif // !_TEXTUREDEPTHVIEW_H_