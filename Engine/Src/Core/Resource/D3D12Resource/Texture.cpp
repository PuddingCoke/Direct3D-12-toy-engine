﻿#include<Gear/Core/Resource/D3D12Resource/Texture.h>

Texture::Texture(const uint32_t width, const uint32_t height, const DXGI_FORMAT format, const uint32_t arraySize, const uint32_t mipLevels, const bool stateTracking, const D3D12_RESOURCE_FLAGS resFlags, const D3D12_CLEAR_VALUE* const clearValue) :
	Resource(CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE, CD3DX12_RESOURCE_DESC::Tex2D(format, width, height, arraySize, mipLevels, 1, 0, resFlags), stateTracking, D3D12_RESOURCE_STATE_COPY_DEST, clearValue),
	width(width),
	height(height),
	arraySize(arraySize),
	mipLevels(mipLevels),
	format(format),
	globalState(std::make_shared<STATES>(STATES{ D3D12_RESOURCE_STATE_COPY_DEST,std::vector<uint32_t>(mipLevels,D3D12_RESOURCE_STATE_COPY_DEST) })),
	internalState(STATES{ D3D12_RESOURCE_STATE_COPY_DEST,std::vector<uint32_t>(mipLevels,D3D12_RESOURCE_STATE_COPY_DEST) }),
	transitionState(STATES{ D3D12_RESOURCE_STATE_UNKNOWN,std::vector<uint32_t>(mipLevels,D3D12_RESOURCE_STATE_UNKNOWN) })
{

}

Texture::Texture(const ComPtr<ID3D12Resource>& texture, const bool stateTracking, const uint32_t initialState) :
	Resource(texture, stateTracking)
{
	D3D12_RESOURCE_DESC desc = getResource()->GetDesc();
	width = static_cast<uint32_t>(desc.Width);
	height = desc.Height;
	arraySize = desc.DepthOrArraySize;
	mipLevels = desc.MipLevels;
	format = desc.Format;

	globalState = std::make_shared<STATES>(STATES{ initialState,std::vector<uint32_t>(mipLevels,initialState) });
	internalState = STATES{ initialState,std::vector<uint32_t>(mipLevels,initialState) };
	transitionState = STATES{ D3D12_RESOURCE_STATE_UNKNOWN,std::vector<uint32_t>(mipLevels,D3D12_RESOURCE_STATE_UNKNOWN) };
}

Texture::Texture(Texture& tex) :
	Resource(tex),
	width(tex.width),
	height(tex.height),
	arraySize(tex.arraySize),
	mipLevels(tex.mipLevels),
	format(tex.format),
	globalState(tex.globalState),
	internalState(STATES{ D3D12_RESOURCE_STATE_UNKNOWN,std::vector<uint32_t>(mipLevels,D3D12_RESOURCE_STATE_UNKNOWN) }),
	transitionState(STATES{ D3D12_RESOURCE_STATE_UNKNOWN,std::vector<uint32_t>(mipLevels,D3D12_RESOURCE_STATE_UNKNOWN) })
{

}

Texture::~Texture()
{
}

void Texture::updateGlobalStates()
{
	for (uint32_t mipSlice = 0; mipSlice < mipLevels; mipSlice++)
	{
		if (internalState.mipLevelStates[mipSlice] != D3D12_RESOURCE_STATE_UNKNOWN)
		{
			globalState->mipLevelStates[mipSlice] = internalState.mipLevelStates[mipSlice];
		}
	}

	if (internalState.allState != D3D12_RESOURCE_STATE_UNKNOWN)
	{
		globalState->allState = internalState.allState;
	}
	else
	{
		bool uniformState = true;

		const uint32_t tempState = globalState->mipLevelStates[0];

		for (uint32_t i = 0; i < mipLevels; i++)
		{
			if (globalState->mipLevelStates[i] != tempState)
			{
				uniformState = false;
				break;
			}
		}

		if (uniformState)
		{
			globalState->allState = tempState;
		}
		else
		{
			globalState->allState = D3D12_RESOURCE_STATE_UNKNOWN;
		}
	}
}

void Texture::resetInternalStates()
{
	internalState.allState = D3D12_RESOURCE_STATE_UNKNOWN;

	for (uint32_t i = 0; i < mipLevels; i++)
	{
		internalState.mipLevelStates[i] = D3D12_RESOURCE_STATE_UNKNOWN;
	}
}

void Texture::resetTransitionStates()
{
	transitionState.allState = D3D12_RESOURCE_STATE_UNKNOWN;

	for (uint32_t i = 0; i < mipLevels; i++)
	{
		transitionState.mipLevelStates[i] = D3D12_RESOURCE_STATE_UNKNOWN;
	}
}

void Texture::transition(std::vector<D3D12_RESOURCE_BARRIER>& transitionBarriers, std::vector<PendingTextureBarrier>& pendingBarriers)
{
	//check each mipslice state of transitionState
	//if they are the same then we set allState to that state
	if (transitionState.allState == D3D12_RESOURCE_STATE_UNKNOWN)
	{
		bool uniformState = true;

		const uint32_t tempState = transitionState.mipLevelStates[0];

		if (tempState == D3D12_RESOURCE_STATE_UNKNOWN)
		{
			uniformState = false;
		}
		else
		{
			for (uint32_t i = 0; i < mipLevels; i++)
			{
				if (tempState != transitionState.mipLevelStates[i])
				{
					uniformState = false;
					break;
				}
			}
		}

		if (uniformState)
		{
			transitionState.allState = tempState;
		}
	}

	bool finalStateChecking = false;

	//if allState of transitionState is known
	if (transitionState.allState != D3D12_RESOURCE_STATE_UNKNOWN)
	{
		//if texture has multiple mipslices
		if (mipLevels > 1)
		{
			//this is the best case,transitionState.allState and internalState.allState are all known
			if (internalState.allState != D3D12_RESOURCE_STATE_UNKNOWN)
			{
				if (!bitFlagSubset(internalState.allState, transitionState.allState))
				{
					D3D12_RESOURCE_BARRIER barrier = {};
					barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
					barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
					barrier.Transition.pResource = getResource();
					barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
					barrier.Transition.StateBefore = static_cast<D3D12_RESOURCE_STATES>(internalState.allState);
					barrier.Transition.StateAfter = static_cast<D3D12_RESOURCE_STATES>(transitionState.allState);

					transitionBarriers.push_back(barrier);

					internalState.allState = transitionState.allState;

					for (uint32_t i = 0; i < mipLevels; i++)
					{
						internalState.mipLevelStates[i] = transitionState.mipLevelStates[i];
					}
				}
			}
			//internalState.allState equals to D3D12_RESOURCE_STATE_UNKNOWN
			//in this case we need furthur checking
			else
			{
				//first we check if all mipslice states of internalState are all unknown
				bool allStatesUnknown = true;

				for (uint32_t i = 0; i < mipLevels; i++)
				{
					if (internalState.mipLevelStates[i] != D3D12_RESOURCE_STATE_UNKNOWN)
					{
						allStatesUnknown = false;
						break;
					}
				}

				//if mipslice states of internalState are all unknown
				//we set barrier.mipSlice to D3D12_TRANSITION_ALL_MIPLEVELS
				//handle this when commandList is submitted for execution
				if (allStatesUnknown)
				{
					PendingTextureBarrier barrier = {};
					barrier.texture = this;
					barrier.mipSlice = D3D12_TRANSITION_ALL_MIPLEVELS;
					barrier.afterState = transitionState.allState;

					pendingBarriers.push_back(barrier);

					internalState.allState = transitionState.allState;

					for (uint32_t i = 0; i < mipLevels; i++)
					{
						internalState.mipLevelStates[i] = transitionState.mipLevelStates[i];
					}
				}
				//some mipslice states of internalState are known
				//in this case we need to check each mipslice state
				//for unknown internal state we push PendingTextureBarrier
				//for known internal state we push D3D12_RESOURCE_BARRIER
				else
				{
					for (uint32_t mipSlice = 0; mipSlice < mipLevels; mipSlice++)
					{
						//unknown
						if (internalState.mipLevelStates[mipSlice] == D3D12_RESOURCE_STATE_UNKNOWN)
						{
							finalStateChecking = true;

							PendingTextureBarrier barrier = {};
							barrier.texture = this;
							barrier.mipSlice = mipSlice;
							barrier.afterState = transitionState.mipLevelStates[mipSlice];

							pendingBarriers.push_back(barrier);

							internalState.mipLevelStates[mipSlice] = transitionState.mipLevelStates[mipSlice];
						}
						//known
						else
						{
							if (!bitFlagSubset(internalState.mipLevelStates[mipSlice], transitionState.mipLevelStates[mipSlice]))
							{
								finalStateChecking = true;

								for (uint32_t arraySlice = 0; arraySlice < arraySize; arraySlice++)
								{
									D3D12_RESOURCE_BARRIER barrier = {};
									barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
									barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
									barrier.Transition.pResource = getResource();
									barrier.Transition.Subresource = D3D12CalcSubresource(mipSlice, arraySlice, 0, mipLevels, arraySize);
									barrier.Transition.StateBefore = static_cast<D3D12_RESOURCE_STATES>(internalState.mipLevelStates[mipSlice]);
									barrier.Transition.StateAfter = static_cast<D3D12_RESOURCE_STATES>(transitionState.mipLevelStates[mipSlice]);

									transitionBarriers.push_back(barrier);
								}

								internalState.mipLevelStates[mipSlice] = transitionState.mipLevelStates[mipSlice];
							}
						}
					}
				}
			}
		}
		//if texture has only one mipslice
		else
		{
			if (internalState.allState == D3D12_RESOURCE_STATE_UNKNOWN)
			{
				PendingTextureBarrier barrier = {};
				barrier.texture = this;
				barrier.mipSlice = D3D12_TRANSITION_ALL_MIPLEVELS;
				barrier.afterState = transitionState.allState;

				pendingBarriers.push_back(barrier);

				internalState.allState = transitionState.allState;

				internalState.mipLevelStates[0] = transitionState.mipLevelStates[0];
			}
			else
			{
				if (!bitFlagSubset(internalState.allState, transitionState.allState))
				{
					D3D12_RESOURCE_BARRIER barrier = {};
					barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
					barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
					barrier.Transition.pResource = getResource();
					barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
					barrier.Transition.StateBefore = static_cast<D3D12_RESOURCE_STATES>(internalState.allState);
					barrier.Transition.StateAfter = static_cast<D3D12_RESOURCE_STATES>(transitionState.allState);

					transitionBarriers.push_back(barrier);

					internalState.allState = transitionState.allState;

					internalState.mipLevelStates[0] = transitionState.mipLevelStates[0];
				}
			}
		}
	}
	//if allState of transitionState is unknown
	//in this case we need to check both mipslice state of internalState and transitionState
	else
	{
		if (mipLevels > 1)
		{
			for (uint32_t mipSlice = 0; mipSlice < mipLevels; mipSlice++)
			{
				if (transitionState.mipLevelStates[mipSlice] != D3D12_RESOURCE_STATE_UNKNOWN)
				{
					if (internalState.mipLevelStates[mipSlice] == D3D12_RESOURCE_STATE_UNKNOWN)
					{
						finalStateChecking = true;

						PendingTextureBarrier barrier = {};
						barrier.texture = this;
						barrier.mipSlice = mipSlice;
						barrier.afterState = transitionState.mipLevelStates[mipSlice];

						pendingBarriers.push_back(barrier);

						internalState.mipLevelStates[mipSlice] = transitionState.mipLevelStates[mipSlice];
					}
					else
					{
						if (!bitFlagSubset(internalState.mipLevelStates[mipSlice], transitionState.mipLevelStates[mipSlice]))
						{
							finalStateChecking = true;

							for (uint32_t arraySlice = 0; arraySlice < arraySize; arraySlice++)
							{
								D3D12_RESOURCE_BARRIER barrier = {};
								barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
								barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
								barrier.Transition.pResource = getResource();
								barrier.Transition.Subresource = D3D12CalcSubresource(mipSlice, arraySlice, 0, mipLevels, arraySize);
								barrier.Transition.StateBefore = static_cast<D3D12_RESOURCE_STATES>(internalState.mipLevelStates[mipSlice]);
								barrier.Transition.StateAfter = static_cast<D3D12_RESOURCE_STATES>(transitionState.mipLevelStates[mipSlice]);

								transitionBarriers.push_back(barrier);
							}

							internalState.mipLevelStates[mipSlice] = transitionState.mipLevelStates[mipSlice];
						}
					}
				}
			}
		}
		else
		{
			LOGERROR("when transition texture that has only 1 miplevel,its allState must be known");
		}
	}

	//previous operation might make each mipslice state of internal state the same
	//so we need to check each mipslice state of internal state and set internalState.allState base on it
	if (finalStateChecking)
	{
		bool uniformState = true;

		const uint32_t tempState = internalState.mipLevelStates[0];

		if (tempState == D3D12_RESOURCE_STATE_UNKNOWN)
		{
			uniformState = false;
		}
		else
		{
			for (uint32_t i = 0; i < mipLevels; i++)
			{
				if (tempState != internalState.mipLevelStates[i])
				{
					uniformState = false;
					break;
				}
			}
		}

		if (uniformState)
		{
			internalState.allState = tempState;
		}
		else
		{
			internalState.allState = D3D12_RESOURCE_STATE_UNKNOWN;
		}
	}

	//at last reset transition state
	//this is important because we need to make this resource available for subsequent uses
	resetTransitionStates();
}

void Texture::solvePendingBarrier(std::vector<D3D12_RESOURCE_BARRIER>& transitionBarriers, const uint32_t targetMipSlice, const uint32_t targetState)
{
	if (targetMipSlice == D3D12_TRANSITION_ALL_MIPLEVELS)
	{
		if (mipLevels > 1)
		{
			if (globalState->allState != D3D12_RESOURCE_STATE_UNKNOWN)
			{
				if (globalState->allState != targetState)
				{
					D3D12_RESOURCE_BARRIER barrier = {};
					barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
					barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
					barrier.Transition.pResource = getResource();
					barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
					barrier.Transition.StateBefore = static_cast<D3D12_RESOURCE_STATES>(globalState->allState);
					barrier.Transition.StateAfter = static_cast<D3D12_RESOURCE_STATES>(targetState);

					transitionBarriers.push_back(barrier);
				}
			}
			else
			{
				for (uint32_t mipSlice = 0; mipSlice < mipLevels; mipSlice++)
				{
					if (globalState->mipLevelStates[mipSlice] != targetState)
					{
						for (uint32_t arraySlice = 0; arraySlice < arraySize; arraySlice++)
						{
							D3D12_RESOURCE_BARRIER barrier = {};
							barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
							barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
							barrier.Transition.pResource = getResource();
							barrier.Transition.Subresource = D3D12CalcSubresource(mipSlice, arraySlice, 0, mipLevels, arraySize);
							barrier.Transition.StateBefore = static_cast<D3D12_RESOURCE_STATES>(globalState->mipLevelStates[mipSlice]);
							barrier.Transition.StateAfter = static_cast<D3D12_RESOURCE_STATES>(targetState);

							transitionBarriers.push_back(barrier);
						}
					}
				}
			}
		}
		else
		{
			if (globalState->allState != targetState)
			{
				D3D12_RESOURCE_BARRIER barrier = {};
				barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
				barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
				barrier.Transition.pResource = getResource();
				barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
				barrier.Transition.StateBefore = static_cast<D3D12_RESOURCE_STATES>(globalState->allState);
				barrier.Transition.StateAfter = static_cast<D3D12_RESOURCE_STATES>(targetState);

				transitionBarriers.push_back(barrier);
			}
		}
	}
	else
	{
		if (mipLevels > 1)
		{
			if (globalState->mipLevelStates[targetMipSlice] != targetState)
			{
				for (uint32_t arraySlice = 0; arraySlice < arraySize; arraySlice++)
				{
					D3D12_RESOURCE_BARRIER barrier = {};
					barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
					barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
					barrier.Transition.pResource = getResource();
					barrier.Transition.Subresource = D3D12CalcSubresource(targetMipSlice, arraySlice, 0, mipLevels, arraySize);
					barrier.Transition.StateBefore = static_cast<D3D12_RESOURCE_STATES>(globalState->mipLevelStates[targetMipSlice]);
					barrier.Transition.StateAfter = static_cast<D3D12_RESOURCE_STATES>(targetState);

					transitionBarriers.push_back(barrier);
				}
			}
		}
		else
		{
			LOGERROR("when transition a texture that has only 1 miplevel,pending mipslice must be D3D12_TRANSITION_ALL_MIPLEVELS");
		}
	}
}

uint32_t Texture::getWidth() const
{
	return width;
}

uint32_t Texture::getHeight() const
{
	return height;
}

uint32_t Texture::getArraySize() const
{
	return arraySize;
}

uint32_t Texture::getMipLevels() const
{
	return mipLevels;
}

DXGI_FORMAT Texture::getFormat() const
{
	return format;
}

void Texture::setAllState(const uint32_t state)
{
	if (transitionState.allState == D3D12_RESOURCE_STATE_UNKNOWN)
	{
		if (mipLevels > 1)
		{
			for (uint32_t i = 0; i < mipLevels; i++)
			{
				if (transitionState.mipLevelStates[i] == D3D12_RESOURCE_STATE_UNKNOWN)
				{
					transitionState.mipLevelStates[i] = state;
				}
				else
				{
					transitionState.mipLevelStates[i] = (transitionState.mipLevelStates[i] | state);
				}
			}
		}
		else
		{
			transitionState.allState = state;

			transitionState.mipLevelStates[0] = state;
		}
	}
	else if (!bitFlagSubset(internalState.allState, state))
	{
		transitionState.allState = (transitionState.allState | state);

		for (uint32_t i = 0; i < mipLevels; i++)
		{
			transitionState.mipLevelStates[i] = (transitionState.mipLevelStates[i] | state);
		}
	}
}

void Texture::setMipSliceState(const uint32_t mipSlice, const uint32_t state)
{
	if (mipLevels > 1)
	{
		if (transitionState.mipLevelStates[mipSlice] == D3D12_RESOURCE_STATE_UNKNOWN)
		{
			transitionState.allState = D3D12_RESOURCE_STATE_UNKNOWN;

			transitionState.mipLevelStates[mipSlice] = state;
		}
		else if (!bitFlagSubset(transitionState.mipLevelStates[mipSlice], state))
		{
			transitionState.allState = D3D12_RESOURCE_STATE_UNKNOWN;

			transitionState.mipLevelStates[mipSlice] = (transitionState.mipLevelStates[mipSlice] | state);
		}
	}
	else
	{
		if (transitionState.mipLevelStates[mipSlice] == D3D12_RESOURCE_STATE_UNKNOWN)
		{
			transitionState.allState = state;

			transitionState.mipLevelStates[mipSlice] = state;
		}
		else
		{
			transitionState.allState = (transitionState.allState | state);

			transitionState.mipLevelStates[mipSlice] = (transitionState.mipLevelStates[mipSlice] | state);
		}
	}
}

uint32_t Texture::getAllState() const
{
	return internalState.allState;
}

uint32_t Texture::getMipSliceState(const uint32_t mipSlice) const
{
	return internalState.mipLevelStates[mipSlice];
}

void Texture::pushToTrackingList(std::vector<Texture*>& trackingList)
{
	if (getStateTracking() && !getInTrackingList())
	{
		trackingList.push_back(this);

		Resource::pushToTrackingList();
	}
}
