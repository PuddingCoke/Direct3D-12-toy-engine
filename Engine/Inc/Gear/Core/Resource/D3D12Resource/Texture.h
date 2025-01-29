#pragma once

#ifndef _TEXTURE_H_
#define _TEXTURE_H_

#include"Resource.h"

#include<vector>

class Texture;

struct PendingTextureBarrier
{
	Texture* texture;

	uint32_t mipSlice;

	uint32_t afterState;
};

class Texture :public Resource
{
public:

	Texture() = delete;

	Texture(const Texture&) = delete;

	void operator=(const Texture&) = delete;

	Texture(const uint32_t width, const uint32_t height, const DXGI_FORMAT format, const uint32_t arraySize, const uint32_t mipLevels, const bool stateTracking, const D3D12_RESOURCE_FLAGS resFlags, const D3D12_CLEAR_VALUE* const clearValue = nullptr);

	Texture(const ComPtr<ID3D12Resource>& texture, const bool stateTracking, const uint32_t initialState);

	Texture(Texture&);

	virtual ~Texture();

	void updateGlobalStates() override;

	void resetInternalStates() override;

	void resetTransitionStates() override;

	void transition(std::vector<D3D12_RESOURCE_BARRIER>& transitionBarriers, std::vector<PendingTextureBarrier>& pendingBarriers);

	uint32_t getWidth() const;

	uint32_t getHeight() const;

	uint32_t getArraySize() const;

	uint32_t getMipLevels() const;

	DXGI_FORMAT getFormat() const;

	void setAllState(const uint32_t state);

	void setMipSliceState(const uint32_t mipSlice, const uint32_t state);

	uint32_t getAllState() const;

	uint32_t getMipSliceState(const uint32_t mipSlice) const;

	void pushToTrackingList(std::vector<Texture*>& trackingList);

private:

	friend class RenderEngine;

	uint32_t width;

	uint32_t height;

	uint32_t arraySize;

	uint32_t mipLevels;

	DXGI_FORMAT format;

	struct STATES
	{
		uint32_t allState;

		std::vector<uint32_t> mipLevelStates;
	};

	std::shared_ptr<STATES> globalState;

	STATES internalState;

	STATES transitionState;

};

#endif // !_TEXTURE_H_