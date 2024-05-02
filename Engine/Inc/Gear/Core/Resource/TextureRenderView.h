#pragma once

#ifndef _TEXTURERENDERVIEW_H_
#define _TEXTURERENDERVIEW_H_

#include<Gear/Core/DX/Texture.h>

#include"EngineResource.h"

//versatile texture
//texturecubearray
//texturecube
//texture2darray
//texture2d
//SRV/RTV/UAV
class TextureRenderView :public EngineResource
{
public:

	TextureRenderView(Texture* const texture, const bool isTextureCube, const bool persistent, const DXGI_FORMAT srvFormat, const DXGI_FORMAT uavFormat, const DXGI_FORMAT rtvFormat);

	TextureRenderView(const TextureRenderView&);

	~TextureRenderView();

	ShaderResourceDesc getAllSRVIndex() const;

	ShaderResourceDesc getSRVMipIndex(const UINT mipSlice) const;

	ShaderResourceDesc getUAVMipIndex(const UINT mipSlice) const;

	RenderTargetDesc getRTVMipHandle(const UINT mipSlice) const;

	ClearUAVDesc getClearUAVDesc(const UINT mipSlice) const;

	Texture* getTexture() const;

	void copyDescriptors() override;

	const bool hasRTV;

	const bool hasUAV;

private:

	UINT allSRVIndex;

	UINT srvMipIndexStart;

	UINT uavMipIndexStart;

	D3D12_CPU_DESCRIPTOR_HANDLE rtvMipHandleStart;

	D3D12_GPU_DESCRIPTOR_HANDLE viewGPUHandleStart;

	D3D12_CPU_DESCRIPTOR_HANDLE viewCPUHandleStart;

	Texture* texture;
};

#endif // !_TEXTURERENDERVIEW_H_
