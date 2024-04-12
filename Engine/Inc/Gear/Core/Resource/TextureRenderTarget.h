#pragma once

#ifndef _TEXTURERENDERTARGET_H_
#define _TEXTURERENDERTARGET_H_

#include<Gear/Core/DX/Resource/Texture.h>

#include"EngineResource.h"

//versatile texture
//texturecubearray
//texturecube
//texture2darray
//texture2d
//SRV/RTV/UAV
class TextureRenderTarget :public EngineResource
{
public:

	TextureRenderTarget(Texture* const texture, const bool isTextureCube, const bool persistent, const DXGI_FORMAT srvFormat, const DXGI_FORMAT uavFormat, const DXGI_FORMAT rtvFormat);

	TextureRenderTarget(const TextureRenderTarget&);

	~TextureRenderTarget();

	ShaderResourceDesc getAllSRVIndex() const;

	ShaderResourceDesc getSRVMipIndex(const UINT mipSlice) const;

	ShaderResourceDesc getUAVMipIndex(const UINT mipSlice) const;

	RenderTargetDesc getRTVMipHandle(const UINT mipSlice) const;

	Texture* getTexture() const;

	void copyDescriptors();

private:

	void createViews(const DXGI_FORMAT srvFormat, const DXGI_FORMAT uavFormat, const DXGI_FORMAT rtvFormat, const bool isTextureCube, const bool persistent);

	//offset 1 for each mipSlice
	//since 0th position is all srv
	UINT allSRVIndex;

	UINT srvMipIndexStart;

	UINT uavMipIndexStart;

	std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> rtvMipHandles;

	CD3DX12_CPU_DESCRIPTOR_HANDLE srvuavDescriptorHandleStart;

	UINT numSRVUAVDescriptors;

	Texture* texture;
};

#endif // !_TEXTURERENDERTARGET_H_
