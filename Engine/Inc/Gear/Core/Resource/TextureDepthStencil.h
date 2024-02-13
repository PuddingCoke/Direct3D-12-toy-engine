#pragma once

#ifndef _TEXTUREDEPTHSTENCIL_H_
#define _TEXTUREDEPTHSTENCIL_H_

#include<Gear/Core/DX/Resource/Texture.h>


class TextureDepthStencil
{
public:

	//supported types
	//R32_TYPELESS 
	//R16_TYPELESS 
	//R32G8X24_TYPELESS 
	//R24G8_TYPELESS
	TextureDepthStencil(const UINT width, const UINT height, const DXGI_FORMAT resFormat, const UINT arraySize, const UINT mipLevels, const bool isTextureCube);

	TextureDepthStencil(const TextureDepthStencil&);

	~TextureDepthStencil();

	ShaderResourceDesc getAllDepthIndex() const;

	ShaderResourceDesc getAllStencilIndex() const;

	ShaderResourceDesc getDepthMipIndex(const UINT mipSlice) const;

	ShaderResourceDesc getStencilMipIndex(const UINT mipSlice) const;

	DepthStencilDesc getDSVMipHandle(const UINT mipSlice) const;

	Texture* getTexture() const;

private:

	UINT allDepthIndex;

	UINT allStencilIndex;

	UINT depthSliceStart;

	UINT stencilSliceStart;

	std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> dsvMipHandles;

	Texture* texture;

};


#endif // !_TEXTUREDEPTHSTENCIL_H_
