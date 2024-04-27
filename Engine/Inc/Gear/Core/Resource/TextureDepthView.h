#pragma once

#ifndef _TEXTUREDEPTHVIEW_H_
#define _TEXTUREDEPTHVIEW_H_

#include<Gear/Core/DX/Resource/Texture.h>

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

	ShaderResourceDesc getDepthMipIndex(const UINT mipSlice) const;

	ShaderResourceDesc getStencilMipIndex(const UINT mipSlice) const;

	DepthStencilDesc getDSVMipHandle(const UINT mipSlice) const;

	Texture* getTexture() const;

	void copyDescriptors() override;

private:

	UINT allDepthIndex;

	UINT allStencilIndex;

	UINT depthMipIndexStart;

	UINT stencilMipIndexStart;

	std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> dsvMipHandles;

	Texture* texture;
};


#endif // !_TEXTUREDEPTHVIEW_H_
