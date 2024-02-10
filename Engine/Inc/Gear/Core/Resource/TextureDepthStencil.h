#pragma once

#ifndef _TEXTUREDEPTHSTENCIL_H_
#define _TEXTUREDEPTHSTENCIL_H_

#include<Gear/Core/DX/Resource/Texture.h>


class TextureDepthStencil
{
public:

	//R32_TYPELESS R16_TYPELESS R32G8X24_TYPELESS R24G8_TYPELESS supported
	TextureDepthStencil(const UINT width, const UINT height, const DXGI_FORMAT resFormat, const UINT arraySize, const UINT mipLevels, const bool isTextureCube);

	TextureDepthStencil(const TextureDepthStencil&);

	~TextureDepthStencil();

	TransitionDesc getAllDepthIndex() const;

	TransitionDesc getAllStencilIndex() const;

	TransitionDesc getDepthMipIndex(const UINT mipSlice) const;

	TransitionDesc getStencilMipIndex(const UINT mipSlice) const;

	TransitionDesc getDSVMipHandle(const UINT mipSlice) const;

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
