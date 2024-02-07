#pragma once

#ifndef _TEXTURE_H_
#define _TEXTURE_H_

#include"Resource.h"

class Texture;

struct PendingTextureBarrier
{
	Texture* texture;

	UINT mipSlice;

	UINT afterState;
};

class Texture :public Resource
{
public:

	Texture(const UINT width, const UINT height, const DXGI_FORMAT format, const UINT arraySize, const UINT mipLevels, const bool stateTracking);

	Texture(Texture&);

	void operator=(const Texture&) = delete;

	Texture(const Texture&) = delete;

	virtual ~Texture();

	void updateGlobalStates() override;

	void resetInternalStates() override;

	void resetTransitionStates() override;

	void pushBarriersAndStateChanging(std::vector<D3D12_RESOURCE_BARRIER>& transitionBarriers, std::vector<PendingTextureBarrier>& pendingBarriers);

private:

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
