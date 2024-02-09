#pragma once

#ifndef _TEXTURE_H_
#define _TEXTURE_H_

#include"Resource.h"
#include"UploadHeap.h"

#include<Gear/Utils/Utils.h>

#include<stb_image/stb_image.h>
#include<DDSTextureLoader/DDSTextureLoader12.h>

class Texture;

constexpr UINT D3D12_TRANSITION_ALL_MIPLEVELS = 0xFFFFFFFF;

struct PendingTextureBarrier
{
	Texture* texture;

	UINT mipSlice;

	UINT afterState;
};

//SRV/UAV/RTV or DSV/SRV
//TextureCube RTV/SRV 

class Texture :public Resource
{
public:

	Texture(Texture&);

	void operator=(const Texture&) = delete;

	Texture(const Texture&) = delete;

	virtual ~Texture();

	void updateGlobalStates() override;

	void resetInternalStates() override;

	void resetTransitionStates() override;

	void transition(std::vector<D3D12_RESOURCE_BARRIER>& transitionBarriers, std::vector<PendingTextureBarrier>& pendingBarriers);

private:

	Texture(const UINT width, const UINT height, const DXGI_FORMAT format, const UINT arraySize, const UINT mipLevels, const bool stateTracking);

	Texture(const std::string filePath, ID3D12GraphicsCommandList6* commandList, std::vector<Resource*>& transientResourcePool,const bool stateTracking);

	friend class RenderEngine;

	struct STATES
	{
		UINT allState;

		std::vector<UINT> mipLevelStates;
	};

	UINT width;

	UINT height;

	UINT arraySize;

	UINT mipLevels;

	DXGI_FORMAT format;

	std::shared_ptr<STATES> globalState;

	STATES internalState;

	STATES transitionState;

};

#endif // !_TEXTURE_H_
