#pragma once

#ifndef _TEXTURERENDERTARGET_H_
#define _TEXTURERENDERTARGET_H_

#include<Gear/Core/DX/Resource/Texture.h>

//versatile texture
//texturecubearray
//texturecube
//texture2darray
//texture2d
//SRV/RTV/UAV
class TextureRenderTarget
{
public:

	TextureRenderTarget(const TextureCreationFlags flags, const UINT width, const UINT height, const DXGI_FORMAT format, const UINT arraySize, const UINT mipLevels,const bool isTextureCube);

	TextureRenderTarget(const TextureCreationFlags flags, const std::string filePath, ID3D12GraphicsCommandList6* commandList, std::vector<Resource*>& transientResourcePool, const bool isTextureCube);

	TextureRenderTarget(const TextureRenderTarget&);

	~TextureRenderTarget();

	UINT getAllSRVIndex() const;

	UINT getSRVMipIndex(const UINT mipSlice) const;

	UINT getUAVMipIndex(const UINT mipSlice) const;

	D3D12_CPU_DESCRIPTOR_HANDLE getRTVMipHandle(const UINT mipSlice) const;

	Texture* getTexture() const;

private:

	//offset 1 for each mipslice
	//since 0th position is all srv
	UINT allSRVIndex;

	UINT srvSliceStart;

	UINT uavSliceStart;

	std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> rtvHandles;

	Texture* texture;

};

#endif // !_TEXTURERENDERTARGET_H_
