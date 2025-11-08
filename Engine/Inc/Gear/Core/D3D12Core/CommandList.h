#pragma once

#ifndef _GEAR_CORE_D3D12CORE_COMMANDLIST_H_
#define _GEAR_CORE_D3D12CORE_COMMANDLIST_H_

#include"DescriptorHeap.h"

#include"PipelineState.h"

#include"RootSignature.h"

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
			class CommandList
			{
			public:

				CommandList() = delete;

				CommandList(const CommandList&) = delete;

				void operator=(const CommandList&) = delete;

				CommandList(const D3D12_COMMAND_LIST_TYPE type);

				~CommandList();

				void resourceBarrier(const uint32_t numBarriers, const D3D12_RESOURCE_BARRIER* const pBarriers) const;

				ID3D12GraphicsCommandList6* get() const;

				void open() const;

				void close() const;

				void setDescriptorHeap(ID3D12DescriptorHeap* const resourceHeap, ID3D12DescriptorHeap* const samplerHeap) const;

				void setPipelineState(ID3D12PipelineState* const pipelineState) const;

				void setGraphicsRootSignature(ID3D12RootSignature* const rootSignature) const;

				void setComputeRootSignature(ID3D12RootSignature* const rootSignature) const;

				void setGraphicsRootConstantBuffer(const uint32_t rootParameterIndex, const D3D12_GPU_VIRTUAL_ADDRESS bufferLocation) const;

				void setComputeRootConstantBuffer(const uint32_t rootParameterIndex, const D3D12_GPU_VIRTUAL_ADDRESS bufferLocation) const;

				void setGraphicsRootConstants(const uint32_t rootParameterIndex, const uint32_t num32BitValuesToSet, const void* const pSrcData, const uint32_t destOffsetIn32BitValues) const;

				void setComputeRootConstants(const uint32_t rootParameterIndex, const uint32_t num32BitValuesToSet, const void* const pSrcData, const uint32_t destOffsetIn32BitValues) const;

				void drawInstanced(const uint32_t vertexCountPerInstance, const uint32_t instanceCount, const uint32_t startVertexLocation, const uint32_t startInstanceLocation) const;

				void drawIndexedInstanced(const uint32_t indexCountPerInstance, const uint32_t instanceCount, const uint32_t startIndexLocation, const int32_t baseVertexLocation, const uint32_t startInstanceLocation) const;

				void dispatch(const uint32_t threadGroupCountX, const uint32_t threadGroupCountY, const uint32_t threadGroupCountZ) const;

				void setRenderTargets(const uint32_t numRenderTargetDescriptors, const D3D12_CPU_DESCRIPTOR_HANDLE* const pRenderTargetDescriptors, bool singleHandleToDescriptorRange, const D3D12_CPU_DESCRIPTOR_HANDLE* const pDepthStencilDescriptor) const;

				void setDefRenderTarget() const;

				void clearRenderTarget(const D3D12_CPU_DESCRIPTOR_HANDLE renderTargetView, const float colorRGBA[4], const uint32_t numRects, const D3D12_RECT* pRects) const;

				void clearDepthStencil(const D3D12_CPU_DESCRIPTOR_HANDLE depthStencilView, const D3D12_CLEAR_FLAGS clearFlags, const float depth, const uint8_t stencil, const uint32_t numRects, const D3D12_RECT* const pRects) const;

				void setVertexBuffers(const uint32_t startSlot, const uint32_t numViews, const D3D12_VERTEX_BUFFER_VIEW* const pViews) const;

				void setIndexBuffer(const D3D12_INDEX_BUFFER_VIEW* const pView) const;

				void setPrimitiveTopology(const D3D12_PRIMITIVE_TOPOLOGY primitiveTopology) const;

				void setViewports(const uint32_t numViewports, const D3D12_VIEWPORT* const pViewports) const;

				void setScissorRects(const uint32_t numRects, const D3D12_RECT* const pRects) const;

				void clearUnorderedAccessFloat(const D3D12_GPU_DESCRIPTOR_HANDLE viewGPUHandleInCurrentHeap, const D3D12_CPU_DESCRIPTOR_HANDLE viewCPUHandle, ID3D12Resource* const pResource, const float values[4], const uint32_t numRects, const D3D12_RECT* const pRects) const;

				void clearUnorderedAccessUint(const D3D12_GPU_DESCRIPTOR_HANDLE viewGPUHandleInCurrentHeap, const D3D12_CPU_DESCRIPTOR_HANDLE viewCPUHandle, ID3D12Resource* const pResource, const uint32_t values[4], const uint32_t numRects, const D3D12_RECT* const pRects) const;

				void transitionResources();

				void pushResourceTrackList(Resource::D3D12Resource::Texture* const texture);

				void pushResourceTrackList(Resource::D3D12Resource::Buffer* const buffer);

				template<size_t N>
				void setShaderResources(const Resource::D3D12Resource::ShaderResourceDesc(&descs)[N], const uint32_t targetSRVState);

				void copyBufferRegion(Resource::D3D12Resource::Buffer* const dstBuffer, const uint64_t dstOffset, Resource::D3D12Resource::UploadHeap* srcBuffer, const uint64_t srcOffset, const uint64_t numBytes);

				void copyBufferRegion(Resource::D3D12Resource::Buffer* const dstBuffer, const uint64_t dstOffset, Resource::D3D12Resource::Buffer* srcBuffer, const uint64_t srcOffset, const uint64_t numBytes);

				void copyResource(Resource::D3D12Resource::Buffer* const dstBuffer, Resource::D3D12Resource::UploadHeap* const srcBuffer);

				void copyResource(Resource::D3D12Resource::Buffer* const dstBuffer, Resource::D3D12Resource::Buffer* const srcBuffer);

				void copyTextureRegion(Resource::D3D12Resource::Texture* const dstTexture, const uint32_t dstSubresource, Resource::D3D12Resource::Texture* const srcTexture, const uint32_t srcSubresource);

				void solvePendingBarriers(std::vector<D3D12_RESOURCE_BARRIER>& barriers);

				void updateReferredSharedResourceStates();

			private:

				ComPtr<ID3D12CommandAllocator>* allocators;

				ComPtr<ID3D12GraphicsCommandList6> commandList;

				std::vector<Resource::D3D12Resource::D3D12ResourceBase*> referredResources;

				std::vector<Resource::D3D12Resource::Buffer*> transitionBuffers;

				std::vector<Resource::D3D12Resource::Texture*> transitionTextures;

				std::vector<D3D12_RESOURCE_BARRIER> transitionBarriers;

				std::vector<Resource::D3D12Resource::PendingBufferBarrier> pendingBufferBarrier;

				std::vector<Resource::D3D12Resource::PendingTextureBarrier> pendingTextureBarrier;

			};

			template<size_t N>
			inline void CommandList::setShaderResources(const Resource::D3D12Resource::ShaderResourceDesc(&descs)[N], const uint32_t targetSRVState)
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

#endif // !_GEAR_CORE_D3D12CORE_COMMANDLIST_H_
