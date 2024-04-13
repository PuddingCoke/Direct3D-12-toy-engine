#pragma once

#ifndef _TEXTURE_H_
#define _TEXTURE_H_

#include"Resource.h"
#include"UploadHeap.h"

#include<Gear/Utils/Utils.h>
#include<Gear/Utils/Random.h>
#include<Gear/Utils/Math.h>

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

class Texture :public Resource
{
public:

	Texture(const UINT width, const UINT height, const DXGI_FORMAT format, const UINT arraySize, const UINT mipLevels, const bool stateTracking, const D3D12_RESOURCE_FLAGS resFlags);

	Texture(const ComPtr<ID3D12Resource>& texture, const bool stateTracking, const UINT initialState);

	Texture(Texture&);

	void operator=(const Texture&) = delete;

	Texture(const Texture&) = delete;

	virtual ~Texture();

	void updateGlobalStates() override;

	void resetInternalStates() override;

	void resetTransitionStates() override;

	void transition(std::vector<D3D12_RESOURCE_BARRIER>& transitionBarriers, std::vector<PendingTextureBarrier>& pendingBarriers);

	UINT getWidth() const;

	UINT getHeight() const;

	UINT getArraySize() const;

	UINT getMipLevels() const;

	DXGI_FORMAT getFormat() const;

	void setAllState(const UINT state);

	void setMipSliceState(const UINT mipSlice, const UINT state);

private:

	friend class RenderEngine;

	UINT width;

	UINT height;

	UINT arraySize;

	UINT mipLevels;

	DXGI_FORMAT format;

	struct STATES
	{
		UINT allState;

		std::vector<UINT> mipLevelStates;
	};

	std::shared_ptr<STATES> globalState;

	STATES internalState;

	STATES transitionState;

};

#endif // !_TEXTURE_H_
