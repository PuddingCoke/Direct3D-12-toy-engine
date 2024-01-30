#pragma once

#ifndef _TEXTURE_H_
#define _TEXTURE_H_

#include"Resource.h"

class Texture :public Resource
{
public:

	Texture(const UINT width, const UINT height, const DXGI_FORMAT format, const UINT arraySize, const UINT mipLevels, const ResourceUsage usage);

	Texture(const Texture&);

	virtual ~Texture();

	void updateGlobalStates() override;

	void resetInternalStates() override;

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

};

struct PendingTextureBarrier
{
	Texture* texture;

	UINT mipSlice;

	UINT afterState;
};

#endif // !_TEXTURE_H_
