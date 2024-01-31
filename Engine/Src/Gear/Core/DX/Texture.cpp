#include<Gear/Core/DX/Resource/Texture.h>

Texture::Texture(const UINT width, const UINT height, const DXGI_FORMAT format, const UINT arraySize, const UINT mipLevels, const ResourceUsage usage) :
	Resource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE, &CD3DX12_RESOURCE_DESC::Tex2D(format, width, height, arraySize, mipLevels), usage, D3D12_RESOURCE_STATE_COPY_DEST, nullptr),
	width(width),
	height(height),
	arraySize(arraySize),
	mipLevels(mipLevels),
	format(format),
	globalState(std::make_shared<STATES>(STATES{ D3D12_RESOURCE_STATE_COPY_DEST,std::vector<UINT>(mipLevels) })),
	internalState(STATES{ D3D12_RESOURCE_STATE_COPY_DEST,std::vector<UINT>(mipLevels) })
{
	for (UINT i = 0; i < mipLevels; i++)
	{
		(*globalState).mipLevelStates[i] = D3D12_RESOURCE_STATE_COPY_DEST;
		internalState.mipLevelStates[i] = D3D12_RESOURCE_STATE_COPY_DEST;
	}
}

Texture::Texture(const Texture& tex):
	Resource(tex),
	width(tex.width),
	height(tex.height),
	arraySize(tex.arraySize),
	mipLevels(tex.mipLevels),
	format(tex.format),
	globalState(tex.globalState),
	internalState(STATES{D3D12_RESOURCE_STATE_UNKNOWN,std::vector<UINT>(mipLevels)})
{
	for (UINT i = 0; i < mipLevels; i++)
	{
		internalState.mipLevelStates[i] = D3D12_RESOURCE_STATE_UNKNOWN;
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
		UINT tempState = internalState.mipLevelStates[0];

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
		internalState.mipLevelStates[mipLevels] = D3D12_RESOURCE_STATE_UNKNOWN;
	}
}
