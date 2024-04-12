#include<Gear/Core/DX/Resource/Texture.h>

Texture::Texture(const UINT width, const UINT height, const DXGI_FORMAT format, const UINT arraySize, const UINT mipLevels, const bool stateTracking, const D3D12_RESOURCE_FLAGS resFlags) :
	Resource(CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE, CD3DX12_RESOURCE_DESC::Tex2D(format, width, height, arraySize, mipLevels, 1, 0, resFlags), stateTracking, D3D12_RESOURCE_STATE_COMMON, nullptr),
	width(width),
	height(height),
	arraySize(arraySize),
	mipLevels(mipLevels),
	format(format),
	globalState(std::make_shared<STATES>(STATES{ D3D12_RESOURCE_STATE_COMMON,std::vector<UINT>(mipLevels) })),
	internalState(STATES{ D3D12_RESOURCE_STATE_COMMON,std::vector<UINT>(mipLevels) }),
	transitionState(STATES{ D3D12_RESOURCE_STATE_UNKNOWN,std::vector<UINT>(mipLevels) })
{
	for (UINT i = 0; i < mipLevels; i++)
	{
		(*globalState).mipLevelStates[i] = D3D12_RESOURCE_STATE_COMMON;
		internalState.mipLevelStates[i] = D3D12_RESOURCE_STATE_COMMON;
		transitionState.mipLevelStates[i] = D3D12_RESOURCE_STATE_UNKNOWN;
	}
}

Texture::Texture(const ComPtr<ID3D12Resource>& texture, const bool stateTracking) :
	Resource(texture, stateTracking),
	globalState(std::make_shared<STATES>(STATES{ D3D12_RESOURCE_STATE_COMMON,std::vector<UINT>(mipLevels) })),
	internalState(STATES{ D3D12_RESOURCE_STATE_COMMON,std::vector<UINT>(mipLevels) }),
	transitionState(STATES{ D3D12_RESOURCE_STATE_UNKNOWN,std::vector<UINT>(mipLevels) })
{
	D3D12_RESOURCE_DESC desc = getResource()->GetDesc();
	width = desc.Width;
	height = desc.Height;
	arraySize = desc.DepthOrArraySize;
	mipLevels = desc.MipLevels;
	format = desc.Format;

	for (UINT i = 0; i < mipLevels; i++)
	{
		(*globalState).mipLevelStates[i] = D3D12_RESOURCE_STATE_COMMON;
		internalState.mipLevelStates[i] = D3D12_RESOURCE_STATE_COMMON;
		transitionState.mipLevelStates[i] = D3D12_RESOURCE_STATE_UNKNOWN;
	}
}

Texture::Texture(Texture& tex) :
	Resource(tex),
	width(tex.width),
	height(tex.height),
	arraySize(tex.arraySize),
	mipLevels(tex.mipLevels),
	format(tex.format),
	globalState(tex.globalState),
	internalState(STATES{ D3D12_RESOURCE_STATE_UNKNOWN,std::vector<UINT>(mipLevels) }),
	transitionState(STATES{ D3D12_RESOURCE_STATE_UNKNOWN,std::vector<UINT>(mipLevels) })
{
	for (UINT i = 0; i < mipLevels; i++)
	{
		internalState.mipLevelStates[i] = D3D12_RESOURCE_STATE_UNKNOWN;
		transitionState.mipLevelStates[i] = D3D12_RESOURCE_STATE_UNKNOWN;
	}
}

Texture::~Texture()
{
}

void Texture::updateGlobalStates()
{
	if (internalState.allState != D3D12_RESOURCE_STATE_UNKNOWN)
	{
		(*globalState).allState = internalState.allState;
	}
	else
	{
		const UINT tempState = internalState.mipLevelStates[0];

		if (tempState != D3D12_RESOURCE_STATE_UNKNOWN)
		{
			bool uniformState = true;

			for (UINT mipSlice = 0; mipSlice < mipLevels; mipSlice++)
			{
				if (internalState.mipLevelStates[mipSlice] != tempState)
				{
					uniformState = false;
					break;
				}
			}

			if (uniformState)
			{
				(*globalState).allState = tempState;
			}
		}
	}

	for (UINT mipSlice = 0; mipSlice < mipLevels; mipSlice++)
	{
		if (internalState.mipLevelStates[mipSlice] != D3D12_RESOURCE_STATE_UNKNOWN)
		{
			(*globalState).mipLevelStates[mipSlice] = internalState.mipLevelStates[mipSlice];
		}
	}
}

void Texture::resetInternalStates()
{
	internalState.allState = D3D12_RESOURCE_STATE_UNKNOWN;

	for (UINT i = 0; i < mipLevels; i++)
	{
		internalState.mipLevelStates[i] = D3D12_RESOURCE_STATE_UNKNOWN;
	}
}

void Texture::resetTransitionStates()
{
	transitionState.allState = D3D12_RESOURCE_STATE_UNKNOWN;

	for (UINT i = 0; i < mipLevels; i++)
	{
		transitionState.mipLevelStates[i] = D3D12_RESOURCE_STATE_UNKNOWN;
	}
}

void Texture::transition(std::vector<D3D12_RESOURCE_BARRIER>& transitionBarriers, std::vector<PendingTextureBarrier>& pendingBarriers)
{
	if (transitionState.allState == D3D12_RESOURCE_STATE_UNKNOWN)
	{
		bool uniformState = true;

		const UINT tempState = transitionState.mipLevelStates[0];

		if (tempState == D3D12_RESOURCE_STATE_UNKNOWN)
		{
			uniformState = false;
		}
		else
		{
			for (UINT i = 0; i < mipLevels; i++)
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

	if (transitionState.allState != D3D12_RESOURCE_STATE_UNKNOWN)
	{
		if (mipLevels > 1)
		{
			bool allStatesUnknown = true;

			for (UINT i = 0; i < mipLevels; i++)
			{
				if (internalState.mipLevelStates[i] != D3D12_RESOURCE_STATE_UNKNOWN)
				{
					allStatesUnknown = false;
					break;
				}
			}

			if (allStatesUnknown)
			{
				PendingTextureBarrier barrier = {};
				barrier.texture = this;
				barrier.mipSlice = D3D12_TRANSITION_ALL_MIPLEVELS;
				barrier.afterState = transitionState.allState;

				pendingBarriers.push_back(barrier);
			}
			else
			{
				bool uniformState = true;

				if (internalState.allState == D3D12_RESOURCE_STATE_UNKNOWN)
				{
					const UINT tempState = internalState.mipLevelStates[0];

					if (tempState == D3D12_RESOURCE_STATE_UNKNOWN)
					{
						uniformState = false;
					}
					else
					{
						for (UINT i = 0; i < mipLevels; i++)
						{
							if (internalState.mipLevelStates[i] != tempState)
							{
								uniformState = false;
								break;
							}
						}
					}
				}

				if (uniformState)
				{
					if (internalState.mipLevelStates[0] != transitionState.mipLevelStates[0])
					{
						D3D12_RESOURCE_BARRIER barrier = {};
						barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
						barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
						barrier.Transition.pResource = getResource();
						barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
						barrier.Transition.StateBefore = static_cast<D3D12_RESOURCE_STATES>(internalState.mipLevelStates[0]);
						barrier.Transition.StateAfter = static_cast<D3D12_RESOURCE_STATES>(transitionState.mipLevelStates[0]);

						transitionBarriers.push_back(barrier);
					}
				}
				else
				{
					for (UINT mipSlice = 0; mipSlice < mipLevels; mipSlice++)
					{
						if (internalState.mipLevelStates[mipSlice] == D3D12_RESOURCE_STATE_UNKNOWN)
						{
							PendingTextureBarrier barrier = {};
							barrier.texture = this;
							barrier.mipSlice = mipSlice;
							barrier.afterState = transitionState.mipLevelStates[mipSlice];

							pendingBarriers.push_back(barrier);
						}
						else
						{
							if (internalState.mipLevelStates[mipSlice] != transitionState.mipLevelStates[mipSlice])
							{
								for (UINT arraySlice = 0; arraySlice < arraySize; arraySlice++)
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
							}
						}
					}
				}
			}

			internalState.allState = transitionState.allState;

			for (UINT i = 0; i < mipLevels; i++)
			{
				internalState.mipLevelStates[i] = transitionState.mipLevelStates[i];
			}
		}
		else
		{
			if (internalState.allState == D3D12_RESOURCE_STATE_UNKNOWN)
			{
				PendingTextureBarrier barrier = {};
				barrier.texture = this;
				barrier.mipSlice = D3D12_TRANSITION_ALL_MIPLEVELS;
				barrier.afterState = transitionState.allState;

				pendingBarriers.push_back(barrier);
			}
			else
			{
				if (internalState.allState != transitionState.allState)
				{
					D3D12_RESOURCE_BARRIER barrier = {};
					barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
					barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
					barrier.Transition.pResource = getResource();
					barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
					barrier.Transition.StateBefore = static_cast<D3D12_RESOURCE_STATES>(internalState.allState);
					barrier.Transition.StateAfter = static_cast<D3D12_RESOURCE_STATES>(transitionState.allState);

					transitionBarriers.push_back(barrier);
				}
			}

			internalState.allState = transitionState.allState;

			internalState.mipLevelStates[0] = transitionState.mipLevelStates[0];
		}
	}
	else
	{
		if (mipLevels > 1)
		{
			for (UINT mipSlice = 0; mipSlice < mipLevels; mipSlice++)
			{
				if (transitionState.mipLevelStates[mipSlice] != D3D12_RESOURCE_STATE_UNKNOWN)
				{
					if (internalState.mipLevelStates[mipSlice] == D3D12_RESOURCE_STATE_UNKNOWN)
					{
						PendingTextureBarrier barrier = {};
						barrier.texture = this;
						barrier.mipSlice = mipSlice;
						barrier.afterState = transitionState.mipLevelStates[mipSlice];

						pendingBarriers.push_back(barrier);
					}
					else
					{
						if (internalState.mipLevelStates[mipSlice] != transitionState.mipLevelStates[mipSlice])
						{
							for (UINT arraySlice = 0; arraySlice < arraySize; arraySlice++)
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
						}
					}

					internalState.mipLevelStates[mipSlice] = transitionState.mipLevelStates[mipSlice];
				}
			}


			bool uniformState = true;

			const UINT tempState = internalState.mipLevelStates[0];

			if (tempState == D3D12_RESOURCE_STATE_UNKNOWN)
			{
				uniformState = false;
			}
			else
			{
				for (UINT i = 0; i < mipLevels; i++)
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
				internalState.allState = internalState.mipLevelStates[0];
			}
			else
			{
				internalState.allState = D3D12_RESOURCE_STATE_UNKNOWN;
			}
		}
		else
		{
			throw "Transition texture with only 1 miplevel transition states' all state must be known\n";
			//in this case transition state's all state is unknown
			//but for texture has only 1 miplevel transition state's all state must be known
			//so not handling this part 
		}
	}

	resetTransitionStates();
}

UINT Texture::getWidth() const
{
	return width;
}

UINT Texture::getHeight() const
{
	return height;
}

UINT Texture::getArraySize() const
{
	return arraySize;
}

UINT Texture::getMipLevels() const
{
	return mipLevels;
}

DXGI_FORMAT Texture::getFormat() const
{
	return format;
}

void Texture::setAllState(const UINT state)
{
	if (transitionState.allState == D3D12_RESOURCE_STATE_UNKNOWN)
	{
		if (mipLevels > 1)
		{
			for (UINT i = 0; i < mipLevels; i++)
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
	else
	{
		if (transitionState.allState != state)
		{
			transitionState.allState = (transitionState.allState | state);

			for (UINT i = 0; i < mipLevels; i++)
			{
				transitionState.mipLevelStates[i] = (transitionState.mipLevelStates[i] | state);
			}
		}
	}
}

void Texture::setMipSliceState(const UINT mipSlice, const UINT state)
{
	if (mipLevels > 1)
	{
		if (transitionState.mipLevelStates[mipSlice] == D3D12_RESOURCE_STATE_UNKNOWN)
		{
			transitionState.allState = D3D12_RESOURCE_STATE_UNKNOWN;

			transitionState.mipLevelStates[mipSlice] = state;
		}
		else
		{
			if (transitionState.mipLevelStates[mipSlice] != state)
			{
				transitionState.allState = D3D12_RESOURCE_STATE_UNKNOWN;

				transitionState.mipLevelStates[mipSlice] = (transitionState.mipLevelStates[mipSlice] | state);
			}
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
