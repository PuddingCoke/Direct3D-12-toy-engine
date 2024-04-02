#pragma once

#ifndef _GRAPHICSCONTEXT_H_
#define _GRAPHICSCONTEXT_H_

#include<Gear/Core/DX/Resource/Buffer.h>
#include<Gear/Core/DX/Resource/Texture.h>

#include<Gear/Core/CommandList.h>
#include<Gear/Core/Resource/ConstantBuffer.h>
#include<Gear/Core/Resource/VertexBuffer.h>
#include<Gear/Core/Resource/IndexBuffer.h>
#include<Gear/Core/Resource/IndexConstantBuffer.h>

#include<vector>
#include<unordered_set>

class GraphicsContext
{
public:
	
	GraphicsContext();

	~GraphicsContext();

	void copyResource(Buffer* const dstBuffer, Buffer* const srcBuffer);

	void copyResource(Texture* const dstTexture, Texture* const srcTexture);

	void updateBuffer(VertexBuffer* const vb, const void* const data, const UINT size);

	void updateBuffer(IndexBuffer* const ib, const void* const data, const UINT size);

	//per frame global resources transition immediate
	void setGlobalConstantBuffer(const IndexConstantBuffer* const indexBuffer);

	void setGlobalConstantBuffer(const ConstantBuffer* const constantBuffer);

	//4 values availiable
	void setVSConstants(const std::initializer_list<ShaderResourceDesc>& descs, const UINT offset);

	//4 values availiable
	void setVSConstants(const UINT numValues, const void* const data, const UINT offset);

	//4 values availiable
	void setHSConstants(const std::initializer_list<ShaderResourceDesc>& descs, const UINT offset);

	//4 values availiable
	void setHSConstants(const UINT numValues, const void* const data, const UINT offset);

	//4 values availiable
	void setDSConstants(const std::initializer_list<ShaderResourceDesc>& descs, const UINT offset);

	//4 values availiable
	void setDSConstants(const UINT numValues, const void* const data, const UINT offset);

	//4 values availiable
	void setGSConstants(const std::initializer_list<ShaderResourceDesc>& descs, const UINT offset);

	//4 values availiable
	void setGSConstants(const UINT numValues, const void* const data, const UINT offset);

	//16 values availiable
	void setPSConstants(const std::initializer_list<ShaderResourceDesc>& descs, const UINT offset);

	//16 values availiable
	void setPSConstants(const UINT numValues, const void* const data, const UINT offset);

	//4 values availiable
	void setASConstants(const std::initializer_list<ShaderResourceDesc>& descs, const UINT offset);

	//4 values availiable
	void setASConstants(const UINT numValues, const void* const data, const UINT offset);

	//4 values availiable
	void setMSConstants(const std::initializer_list<ShaderResourceDesc>& descs, const UINT offset);

	//4 values availiable
	void setMSConstants(const UINT numValues, const void* const data, const UINT offset);

	//32 values availiable
	void setCSConstants(const std::initializer_list<ShaderResourceDesc>& descs, const UINT offset);

	//32 values availiable
	void setCSConstants(const UINT numValues, const void* const data, const UINT offset);

	void setVSConstantBuffer(const IndexConstantBuffer* const constantBuffer);

	void setVSConstantBuffer(const ConstantBuffer* const constantBuffer);

	void setHSConstantBuffer(const IndexConstantBuffer* const constantBuffer);

	void setHSConstantBuffer(const ConstantBuffer* const constantBuffer);

	void setDSConstantBuffer(const IndexConstantBuffer* const constantBuffer);

	void setDSConstantBuffer(const ConstantBuffer* const constantBuffer);

	void setGSConstantBuffer(const IndexConstantBuffer* const constantBuffer);

	void setGSConstantBuffer(const ConstantBuffer* const constantBuffer);

	void setPSConstantBuffer(const IndexConstantBuffer* const constantBuffer);

	void setPSConstantBuffer(const ConstantBuffer* const constantBuffer);

	void setASConstantBuffer(const IndexConstantBuffer* const constantBuffer);

	void setASConstantBuffer(const ConstantBuffer* const constantBuffer);

	void setMSConstantBuffer(const IndexConstantBuffer* const constantBuffer);

	void setMSConstantBuffer(const ConstantBuffer* const constantBuffer);

	void setCSConstantBuffer(const IndexConstantBuffer* const constantBuffer);

	void setCSConstantBuffer(const ConstantBuffer* const constantBuffer);

	//must call this method after set*SConstants or set*SConstantBuffer
	void transitionResources();

	void setRenderTargets(const std::initializer_list<RenderTargetDesc>& renderTargets, const std::initializer_list<DepthStencilDesc>& depthStencils);

	void setDefRenderTarget() const;

	void clearDefRenderTarget(const FLOAT clearValue[4]) const;

	void setVertexBuffers(const UINT startSlot, const std::initializer_list<VertexBuffer*>& vertexBuffers);

	void setIndexBuffers(const std::initializer_list<IndexBuffer*>& indexBuffers);

	void setTopology(const D3D12_PRIMITIVE_TOPOLOGY topology) const;

	void setViewport(const float width, const float height);

	void setViewport(const UINT width, const UINT height);

	void setScissorRect(const UINT left, const UINT top, const UINT right, const UINT bottom);

	void setScissorRect(const float left, const float top, const float right, const float bottom);

	void setViewportSimple(const float width, const float height);

	void setViewportSimple(const UINT width, const UINT height);

	void setPipelineState(ID3D12PipelineState* const pipelineState) const;

	void clearRenderTarget(const RenderTargetDesc desc, const FLOAT clearValue[4]) const;

	void clearDepthStencil(const DepthStencilDesc desc, const D3D12_CLEAR_FLAGS flags, const FLOAT depth, const UINT8 stencil) const;

	void uavBarrier(const std::initializer_list<Resource*>& resources) const;

	void draw(const UINT vertexCountPerInstance, const UINT instanceCount, const UINT startVertexLocation, const UINT startInstanceLocation) const;

	void dispatch(const UINT threadGroupCountX, const UINT threadGroupCountY, const UINT threadGroupCountZ);

	void begin() const;

	void end() const;

	CommandList* getCommandList() const;

private:

	friend class RenderEngine;

	static ConstantBuffer* globalConstantBuffer;

	void updateReferredResStates();

	void getIndicesFromResourceDescs(const std::initializer_list<ShaderResourceDesc>& descs, UINT* const dst);

	void setGraphicsPipelineResources(const IndexConstantBuffer* const constantBuffer, const UINT targetSRVState);

	void setGraphicsPipelineResources(const std::initializer_list<ShaderResourceDesc>& descs, const UINT targetSRVState);

	void setComputePipelineResources(const IndexConstantBuffer* const constantBuffer);

	void setComputePipelineResources(const std::initializer_list<ShaderResourceDesc>& descs);

	void pushResourceTrackList(Texture* const texture);

	void pushResourceTrackList(Buffer* const buffer);

	D3D12_VIEWPORT vp;

	D3D12_RECT rt;

	std::unordered_set<Resource*> referredResources;

	std::unordered_set<Buffer*> transitionBuffers;

	std::unordered_set<Texture*> transitionTextures;

	std::vector<D3D12_RESOURCE_BARRIER> transitionBarriers;

	std::vector<PendingBufferBarrier> pendingBufferBarrier;

	std::vector<PendingTextureBarrier> pendingTextureBarrier;

	CommandList* const commandList;

};


#endif // !_GRAPHICSCONTEXT_H_
