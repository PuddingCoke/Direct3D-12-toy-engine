#pragma once

#ifndef _GEAR_CORE_D3D12CORE_RESOURCESTATETRACKER_H_
#define _GEAR_CORE_D3D12CORE_RESOURCESTATETRACKER_H_

#include<Gear/Core/Resource/D3D12Resource/Buffer.h>

#include<Gear/Core/Resource/D3D12Resource/Texture.h>

#include<Gear/Core/Resource/D3D12Resource/UploadHeap.h>

#include<Gear/Core/Resource/D3D12Resource/PipelineResourceDesc.h>

namespace Gear
{
	namespace Core
	{
		namespace D3D12Core
		{
			class ResourceStateTracker
			{
			public:

				template<size_t N>
				void setShaderResources(const Resource::D3D12Resource::ShaderResourceDesc(&descs)[N], const uint32_t targetSRVState);

				void solvePendingBarriers(std::vector<D3D12_RESOURCE_BARRIER>& outBarriers);

				void updateReferredSharedResourceStates();

			protected:

				void transitionResources(ID3D12GraphicsCommandList6* const commandList);

			private:

				void pushResourceTrackList(Resource::D3D12Resource::Texture* const texture);

				void pushResourceTrackList(Resource::D3D12Resource::Buffer* const buffer);

				//记录使用过的共享且需要状态追踪的资源
				std::vector<Resource::D3D12Resource::D3D12ResourceBase*> referredResources;

				//暂存需要状态转变的缓冲
				std::vector<Resource::D3D12Resource::Buffer*> transitionBuffers;

				//暂存需要状态转变的纹理
				std::vector<Resource::D3D12Resource::Texture*> transitionTextures;

				//暂存资源屏障
				std::vector<D3D12_RESOURCE_BARRIER> transitionBarriers;

				//以下是待定资源屏障，对于共享且需要状态追踪的资源来说它的内部状态是未知的，即STATE_BEFORE是未知的
				//因此需要待定资源屏障，并让主渲染线程解决这个问题
				
				std::vector<Resource::D3D12Resource::PendingBufferBarrier> pendingBufferBarrier;

				std::vector<Resource::D3D12Resource::PendingTextureBarrier> pendingTextureBarrier;

			};

			template<size_t N>
			inline void ResourceStateTracker::setShaderResources(const Resource::D3D12Resource::ShaderResourceDesc(&descs)[N], const uint32_t targetSRVState)
			{
				for (const Resource::D3D12Resource::ShaderResourceDesc& desc : descs)
				{
					if (desc.type == Resource::D3D12Resource::ShaderResourceDesc::BUFFER)
					{
						Resource::D3D12Resource::Buffer* const buffer = desc.bufferDesc.buffer;

						if (buffer && buffer->getStateTracking())
						{
							pushResourceTrackList(buffer);

							if (desc.state == Resource::D3D12Resource::ShaderResourceDesc::SRV)
							{
								buffer->setState(targetSRVState);
							}
							else if (desc.state == Resource::D3D12Resource::ShaderResourceDesc::UAV)
							{
								buffer->setState(D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

								Resource::D3D12Resource::Buffer* const counterBuffer = desc.bufferDesc.counterBuffer;

								if (counterBuffer)
								{
									pushResourceTrackList(counterBuffer);

									counterBuffer->setState(D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
								}
							}
							else
							{
								buffer->setState(D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
							}
						}
					}
					else if (desc.type == Resource::D3D12Resource::ShaderResourceDesc::TEXTURE)
					{
						Resource::D3D12Resource::Texture* const texture = desc.textureDesc.texture;

						if (texture->getStateTracking())
						{
							pushResourceTrackList(texture);

							if (desc.state == Resource::D3D12Resource::ShaderResourceDesc::SRV)
							{
								if (desc.textureDesc.mipSlice == Resource::D3D12Resource::D3D12_TRANSITION_ALL_MIPLEVELS)
								{
									texture->setAllState(targetSRVState);
								}
								else
								{
									texture->setMipSliceState(desc.textureDesc.mipSlice, targetSRVState);
								}
							}
							else if (desc.state == Resource::D3D12Resource::ShaderResourceDesc::UAV)
							{
								if (desc.textureDesc.mipSlice == Resource::D3D12Resource::D3D12_TRANSITION_ALL_MIPLEVELS)
								{
									texture->setAllState(D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
								}
								else
								{
									texture->setMipSliceState(desc.textureDesc.mipSlice, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
								}
							}
						}
					}
				}
			}

		}
	}
}

#endif // !_GEAR_CORE_D3D12CORE_RESOURCESTATETRACKER_H_
