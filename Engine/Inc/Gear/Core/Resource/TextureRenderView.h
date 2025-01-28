#pragma once

#ifndef _TEXTURERENDERVIEW_H_
#define _TEXTURERENDERVIEW_H_

#include"D3D12Resource/Texture.h"

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

	TextureRenderView() = delete;

	TextureRenderView(Texture* const texture, const bool isTextureCube, const bool persistent, const DXGI_FORMAT srvFormat, const DXGI_FORMAT uavFormat, const DXGI_FORMAT rtvFormat);

	TextureRenderView(const TextureRenderView&);

	~TextureRenderView();

	ShaderResourceDesc getAllSRVIndex() const;

	ShaderResourceDesc getSRVMipIndex(const uint32_t mipSlice) const;

	ShaderResourceDesc getUAVMipIndex(const uint32_t mipSlice) const;

	RenderTargetDesc getRTVMipHandle(const uint32_t mipSlice) const;

	ClearUAVDesc getClearUAVDesc(const uint32_t mipSlice) const;

	Texture* getTexture() const;

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

	Texture* texture;
};

#endif // !_TEXTURERENDERVIEW_H_