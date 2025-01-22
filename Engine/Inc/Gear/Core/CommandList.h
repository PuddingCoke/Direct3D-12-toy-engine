#pragma once

#ifndef _COMMANDLIST_H_
#define _COMMANDLIST_H_

#include<unordered_set>

#include<Gear/Core/CommandAllocator.h>
#include<Gear/Core/Graphics.h>
#include<Gear/Core/DescriptorHeap.h>
#include<Gear/Core/RootSignature.h>

#include<Gear/Core/DX/Buffer.h>
#include<Gear/Core/DX/Texture.h>
#include<Gear/Core/DX/UploadHeap.h>
#include<Gear/Core/DX/ReadbackHeap.h>

template<typename T>
using IsCorrectType = std::enable_if_t<
	std::is_same<T, std::vector<ShaderResourceDesc>>::value ||
	std::is_same<T, std::initializer_list<ShaderResourceDesc>>::value,
	void
>;

class CommandList
{
public:

	CommandList(const D3D12_COMMAND_LIST_TYPE type);
	
	~CommandList();

	void resourceBarrier(const uint32_t numBarriers, const D3D12_RESOURCE_BARRIER* const pBarriers) const;

	ID3D12GraphicsCommandList6* get() const;

	void open() const;

	void close() const;

	void setDescriptorHeap(DescriptorHeap* const resourceHeap, DescriptorHeap* const samplerHeap) const;

	void setGraphicsRootSignature(RootSignature* const rootSignature) const;

	void setComputeRootSignature(RootSignature* const rootSignature) const;

	void transitionResources();

	void pushResourceTrackList(Texture* const texture);

	void pushResourceTrackList(Buffer* const buffer);

	void setAllPipelineResources(const std::vector<ShaderResourceDesc>& descs);

	void setAllPipelineResources(const std::initializer_list<ShaderResourceDesc>& descs);

	void setGraphicsPipelineResources(const std::vector<ShaderResourceDesc>& descs, const uint32_t targetSRVState);

	void setGraphicsPipelineResources(const std::initializer_list<ShaderResourceDesc>& descs, const uint32_t targetSRVState);

	void setComputePipelineResources(const std::vector<ShaderResourceDesc>& descs);

	void setComputePipelineResources(const std::initializer_list<ShaderResourceDesc>& descs);

	void setDefRenderTarget() const;

	void clearDefRenderTarget(const float clearValue[4]) const;

	void setRenderTargets(const std::initializer_list<RenderTargetDesc>& renderTargets, const DepthStencilDesc* const depthStencils);

	void setVertexBuffers(const uint32_t startSlot, const std::initializer_list<VertexBufferDesc>& vertexBuffers);

	void setIndexBuffer(const IndexBufferDesc& indexBuffer);
	
	void copyBufferRegion(Buffer* const dstBuffer, const uint64_t dstOffset, UploadHeap* srcBuffer, const uint64_t srcOffset, const uint64_t numBytes);

	void copyBufferRegion(Buffer* const dstBuffer, const uint64_t dstOffset, Buffer* srcBuffer, const uint64_t srcOffset, const uint64_t numBytes);

	void copyResource(Buffer* const dstBuffer, UploadHeap* const srcBuffer);

	void copyResource(Buffer* const dstBuffer, Buffer* const srcBuffer);

	void copyTextureRegion(Texture* const dstTexture, const uint32_t dstSubresource, Texture* const srcTexture, const uint32_t srcSubresource);

	void uavBarrier(const std::initializer_list<Resource*>& resources);

	void clearUnorderedAccessView(const ClearUAVDesc& desc, const float values[4]);

	void clearUnorderedAccessView(const ClearUAVDesc& desc, const uint32_t values[4]);

private:

	friend class RenderEngine;

	void updateReferredResStates();

	template<typename T>
	IsCorrectType<T> setPipelineResources(const T& descs, const uint32_t targetSRVState);

	static D3D12_CPU_DESCRIPTOR_HANDLE backBufferHandle;

	CommandAllocator** allocators;

	ComPtr<ID3D12GraphicsCommandList6> commandList;

	std::unordered_set<Resource*> referredResources;

	std::unordered_set<Buffer*> transitionBuffers;

	std::unordered_set<Texture*> transitionTextures;

	std::vector<D3D12_RESOURCE_BARRIER> transitionBarriers;

	std::vector<PendingBufferBarrier> pendingBufferBarrier;

	std::vector<PendingTextureBarrier> pendingTextureBarrier;

	std::vector<D3D12_RESOURCE_BARRIER> transientUAVBarriers;

	std::vector<D3D12_VERTEX_BUFFER_VIEW> transientVBViews;

	std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> transientRTVHandles;

};

#endif // !_COMMANDLIST_H_

template<typename T>
inline IsCorrectType<T> CommandList::setPipelineResources(const T& descs, const uint32_t targetSRVState)
{
	for (const ShaderResourceDesc& desc : descs)
	{
		if (desc.type == ShaderResourceDesc::BUFFER)
		{
			if (desc.state != ShaderResourceDesc::CBV) 
			{
				Buffer* const buffer = desc.bufferDesc.buffer;

				if (buffer->getStateTracking())
				{
					pushResourceTrackList(buffer);

					if (desc.state == ShaderResourceDesc::SRV)
					{
						buffer->setState(targetSRVState);
					}
					else if (desc.state == ShaderResourceDesc::UAV)
					{
						buffer->setState(D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

						Buffer* const counterBuffer = desc.bufferDesc.counterBuffer;

						if (counterBuffer)
						{
							pushResourceTrackList(counterBuffer);

							counterBuffer->setState(D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
						}
					}
				}
			}
		}
		else if (desc.type == ShaderResourceDesc::TEXTURE)
		{
			Texture* const texture = desc.textureDesc.texture;

			if (texture->getStateTracking())
			{
				pushResourceTrackList(texture);

				if (desc.state == ShaderResourceDesc::SRV)
				{
					if (desc.textureDesc.mipSlice == D3D12_TRANSITION_ALL_MIPLEVELS)
					{
						texture->setAllState(targetSRVState);
					}
					else
					{
						texture->setMipSliceState(desc.textureDesc.mipSlice, targetSRVState);
					}
				}
				else if (desc.state == ShaderResourceDesc::UAV)
				{
					if (desc.textureDesc.mipSlice == D3D12_TRANSITION_ALL_MIPLEVELS)
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
