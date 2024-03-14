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

	enum class TextureType
	{
		NOISE,//4 channel [0,1]
		GAUSS //mean 0 standard deviation 1
	};

	Texture(const UINT width, const UINT height, const DXGI_FORMAT format, const UINT arraySize, const UINT mipLevels, const bool stateTracking, const D3D12_RESOURCE_FLAGS resFlags);

	Texture(const std::string filePath, ID3D12GraphicsCommandList6* commandList, std::vector<Resource*>* transientResourcePool, const bool stateTracking, const D3D12_RESOURCE_FLAGS resFlags);

	Texture(const UINT width, const UINT height, const TextureType type, ID3D12GraphicsCommandList6* commandList, std::vector<Resource*>* transientResourcePool);

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

protected:

	struct STATES
	{
		UINT allState;

		std::vector<UINT> mipLevelStates;
	};

	STATES transitionState;

private:

	friend class RenderEngine;

	friend class GraphicsContext;

	UINT width;

	UINT height;

	UINT arraySize;

	UINT mipLevels;

	DXGI_FORMAT format;

	std::shared_ptr<STATES> globalState;

	STATES internalState;

};

#endif // !_TEXTURE_H_
