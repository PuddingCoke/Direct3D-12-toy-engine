#include<Gear/Core/DX/Resource/Texture.h>

Texture::Texture(const UINT width, const UINT height, const DXGI_FORMAT format, const UINT arraySize, const UINT mipLevels, const bool stateTracking) :
	Resource(CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE, CD3DX12_RESOURCE_DESC::Tex2D(format, width, height, arraySize, mipLevels), stateTracking, D3D12_RESOURCE_STATE_COPY_DEST, nullptr),
	width(width),
	height(height),
	arraySize(arraySize),
	mipLevels(mipLevels),
	format(format),
	globalState(std::make_shared<STATES>(STATES{ D3D12_RESOURCE_STATE_COPY_DEST,std::vector<UINT>(mipLevels) })),
	internalState(STATES{ D3D12_RESOURCE_STATE_COPY_DEST,std::vector<UINT>(mipLevels) }),
	transitionState(STATES{ D3D12_RESOURCE_STATE_UNKNOWN,std::vector<UINT>(mipLevels) })
{
	for (UINT i = 0; i < mipLevels; i++)
	{
		(*globalState).mipLevelStates[i] = D3D12_RESOURCE_STATE_COPY_DEST;
		internalState.mipLevelStates[i] = D3D12_RESOURCE_STATE_COPY_DEST;
		transitionState.mipLevelStates[i] = D3D12_RESOURCE_STATE_UNKNOWN;
	}
}

Texture::Texture(const std::string filePath, ID3D12GraphicsCommandList6* commandList, std::vector<Resource*>* transientResourcePool, const bool stateTracking) :
	Resource(stateTracking)
{
	std::string fileExtension = Utils::File::getExtension(filePath);

	for (char& c : fileExtension)
	{
		c = static_cast<char>(std::tolower(c));
	}

	if (fileExtension == "jpg" || fileExtension == "jpeg" || fileExtension == "png")
	{
		int textureWidth, textureHeight, channels;

		unsigned char* pixels = stbi_load(filePath.c_str(), &textureWidth, &textureHeight, &channels, 4);

		width = static_cast<UINT>(textureWidth);
		height = static_cast<UINT>(textureHeight);
		format = DXGI_FORMAT_R8G8B8A8_UNORM;
		mipLevels = 1;
		arraySize = 1;

		if (pixels)
		{
			D3D12_RESOURCE_DESC desc = {};
			desc.Width = width;
			desc.Height = height;
			desc.DepthOrArraySize = arraySize;
			desc.MipLevels = mipLevels;
			desc.Flags = D3D12_RESOURCE_FLAG_NONE;
			desc.Format = format;
			desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
			desc.SampleDesc.Count = 1;
			desc.SampleDesc.Quality = 0;

			createResource(CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE, desc, D3D12_RESOURCE_STATE_COPY_DEST, nullptr);

			const UINT64 uploadHeapSize = GetRequiredIntermediateSize(getResource(), 0, 1);

			UploadHeap* uploadHeap = new UploadHeap(uploadHeapSize);

			transientResourcePool->push_back(uploadHeap);

			D3D12_SUBRESOURCE_DATA subresourceData = {};
			subresourceData.pData = pixels;
			subresourceData.RowPitch = width * 4u;
			subresourceData.SlicePitch = subresourceData.RowPitch * height;

			UpdateSubresources(commandList, getResource(), uploadHeap->getResource(), 0, 0, 1, &subresourceData);

			stbi_image_free(pixels);
		}
		else
		{
			throw "Cannot open file named " + filePath;
		}
	}
	else if (fileExtension == "hdr")
	{
		int textureWidth, textureHeight, channels;

		float* pixels = stbi_loadf(filePath.c_str(), &textureWidth, &textureHeight, &channels, 4);

		width = static_cast<UINT>(textureWidth);
		height = static_cast<UINT>(textureHeight);
		format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		mipLevels = 1;
		arraySize = 1;

		if (pixels)
		{
			D3D12_RESOURCE_DESC desc = {};
			desc.Width = width;
			desc.Height = height;
			desc.DepthOrArraySize = arraySize;
			desc.MipLevels = mipLevels;
			desc.Flags = D3D12_RESOURCE_FLAG_NONE;
			desc.Format = format;
			desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
			desc.SampleDesc.Count = 1;
			desc.SampleDesc.Quality = 0;

			createResource(CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE, desc, D3D12_RESOURCE_STATE_COPY_DEST, nullptr);

			const UINT64 uploadHeapSize = GetRequiredIntermediateSize(getResource(), 0, 1);

			UploadHeap* uploadHeap = new UploadHeap(uploadHeapSize);

			transientResourcePool->push_back(uploadHeap);

			D3D12_SUBRESOURCE_DATA subresourceData = {};
			subresourceData.pData = pixels;
			subresourceData.RowPitch = width * 16u;
			subresourceData.SlicePitch = subresourceData.RowPitch * height;

			UpdateSubresources(commandList, getResource(), uploadHeap->getResource(), 0, 0, 1, &subresourceData);

			stbi_image_free(pixels);
		}
		else
		{
			throw "Cannot open file named " + filePath;
		}
	}
	else if (fileExtension == "dds")
	{
		std::wstring wFilePath = std::wstring(filePath.begin(), filePath.end());

		std::unique_ptr<uint8_t[]> ddsData;

		std::vector<D3D12_SUBRESOURCE_DATA> subresources;

		DirectX::LoadDDSTextureFromFile(GraphicsDevice::get(), wFilePath.c_str(), releaseAndGet(), ddsData, subresources);

		const UINT64 uploadHeapSize = GetRequiredIntermediateSize(getResource(), 0, static_cast<UINT>(subresources.size()));

		UploadHeap* uploadHeap = new UploadHeap(uploadHeapSize);

		transientResourcePool->push_back(uploadHeap);

		UpdateSubresources(commandList, getResource(), uploadHeap->getResource(), 0, 0, static_cast<UINT>(subresources.size()), subresources.data());

		D3D12_RESOURCE_DESC desc = {};

		desc = getResource()->GetDesc();

		width = desc.Width;
		height = desc.Height;
		arraySize = desc.DepthOrArraySize;
		mipLevels = desc.MipLevels;
		format = desc.Format;
	}
	else
	{
		throw "Format not supported";
	}

	if (stateTracking)
	{
		globalState = std::make_shared<STATES>(STATES{ D3D12_RESOURCE_STATE_COPY_DEST,std::vector<UINT>(mipLevels) });
		internalState = STATES{ D3D12_RESOURCE_STATE_COPY_DEST,std::vector<UINT>(mipLevels) };
		transitionState = STATES{ D3D12_RESOURCE_STATE_UNKNOWN,std::vector<UINT>(mipLevels) };

		for (UINT i = 0; i < mipLevels; i++)
		{
			(*globalState).mipLevelStates[i] = D3D12_RESOURCE_STATE_COPY_DEST;
			internalState.mipLevelStates[i] = D3D12_RESOURCE_STATE_COPY_DEST;
			transitionState.mipLevelStates[i] = D3D12_RESOURCE_STATE_UNKNOWN;
		}
	}
	else
	{
		globalState = std::make_shared<STATES>(STATES{ D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE,std::vector<UINT>(mipLevels) });
		internalState = STATES{ D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE,std::vector<UINT>(mipLevels) };
		transitionState = STATES{ D3D12_RESOURCE_STATE_UNKNOWN,std::vector<UINT>(mipLevels) };

		for (UINT i = 0; i < mipLevels; i++)
		{
			(*globalState).mipLevelStates[i] = D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE;
			internalState.mipLevelStates[i] = D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE;
			transitionState.mipLevelStates[i] = D3D12_RESOURCE_STATE_UNKNOWN;
		}

		D3D12_RESOURCE_BARRIER barrier = {};
		barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		barrier.Transition.pResource = getResource();
		barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
		barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE;

		commandList->ResourceBarrier(1, &barrier);
	}

	std::cout << "[class Texture] create texture at " << filePath << " succeeded\n";
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
