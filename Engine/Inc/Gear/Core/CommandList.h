#pragma once

#ifndef _COMMANDLIST_H_
#define _COMMANDLIST_H_

#include<unordered_set>

#include<Gear/Core/CommandAllocator.h>
#include<Gear/Core/Graphics.h>
#include<Gear/Core/GlobalDescriptorHeap.h>
#include<Gear/Core/GlobalRootSignature.h>

#include<Gear/Core/DX/Resource/Buffer.h>
#include<Gear/Core/DX/Resource/Texture.h>
#include<Gear/Core/DX/Resource/UploadHeap.h>
#include<Gear/Core/DX/Resource/ReadbackHeap.h>

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

	void reset() const;

	void close() const;

	void setDescriptorHeap(DescriptorHeap* const resourceHeap, DescriptorHeap* const samplerHeap) const;

	void setGraphicsRootSignature(RootSignature* const rootSignature) const;

	void setComputeRootSignature(RootSignature* const rootSignature) const;

	ID3D12GraphicsCommandList6* get() const;

	void transitionResources();

	void pushResourceTrackList(Texture* const texture);

	void pushResourceTrackList(Buffer* const buffer);

	void setAllPipelineResources(const std::vector<ShaderResourceDesc>& descs);

	void setAllPipelineResources(const std::initializer_list<ShaderResourceDesc>& descs);

	void setGraphicsPipelineResources(const std::vector<ShaderResourceDesc>& descs, const UINT targetSRVState);

	void setGraphicsPipelineResources(const std::initializer_list<ShaderResourceDesc>& descs, const UINT targetSRVState);

	void setComputePipelineResources(const std::vector<ShaderResourceDesc>& descs);

	void setComputePipelineResources(const std::initializer_list<ShaderResourceDesc>& descs);

	void setRenderTargets(const std::initializer_list<RenderTargetDesc>& renderTargets, const std::initializer_list<DepthStencilDesc>& depthStencils);

	void setVertexBuffers(const UINT startSlot, const std::initializer_list<VertexBufferDesc>& vertexBuffers);

	void setIndexBuffer(const IndexBufferDesc indexBuffer);
	
	void copyBufferRegion(Buffer* const dstBuffer, const UINT64 dstOffset, UploadHeap* srcBuffer, const UINT64 srcOffset, const UINT64 numBytes);

	void copyBufferRegion(Buffer* const dstBuffer, const UINT64 dstOffset, Buffer* srcBuffer, const UINT64 srcOffset, const UINT64 numBytes);

	void copyResource(Buffer* const dstBuffer, UploadHeap* const srcBuffer);

	void copyResource(Buffer* const dstBuffer, Buffer* const srcBuffer);

	void uavBarrier(const std::initializer_list<Resource*>& resources);

private:

	friend class RenderEngine;

	void updateReferredResStates();

	template<typename T>
	IsCorrectType<T> setPipelineResources(const T& descs, const UINT targetSRVState);

	CommandAllocator* allocators[Graphics::FrameBufferCount];

	ComPtr<ID3D12GraphicsCommandList6> commandList;

	std::unordered_set<Resource*> referredResources;

	std::unordered_set<Buffer*> transitionBuffers;

	std::unordered_set<Texture*> transitionTextures;

	std::vector<D3D12_RESOURCE_BARRIER> transitionBarriers;

	std::vector<PendingBufferBarrier> pendingBufferBarrier;

	std::vector<PendingTextureBarrier> pendingTextureBarrier;

};

#endif // !_COMMANDLIST_H_

template<typename T>
inline IsCorrectType<T> CommandList::setPipelineResources(const T& descs, const UINT targetSRVState)
{
	for (const ShaderResourceDesc& desc : descs)
	{
		if (desc.type == ShaderResourceDesc::BUFFER)
		{
			Buffer* const buffer = desc.bufferDesc.buffer;

			if (buffer->getStateTracking() && desc.state != ShaderResourceDesc::CBV)
			{
				pushResourceTrackList(buffer);

				if (desc.state == ShaderResourceDesc::SRV)
				{
					buffer->setState(targetSRVState);
				}
				else if (desc.state == ShaderResourceDesc::UAV)
				{
					buffer->setState(D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
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
