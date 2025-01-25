#pragma once

#ifndef _GRAPHICSCONTEXT_H_
#define _GRAPHICSCONTEXT_H_

#include<Gear/Core/CommandList.h>

#include<Gear/Core/Resource/BufferView.h>

#include<Gear/Core/Resource/ConstantBuffer.h>

#include<Gear/Core/Resource/IndexConstantBuffer.h>

class GraphicsContext
{
public:

	GraphicsContext(const GraphicsContext&) = delete;

	void operator=(const GraphicsContext&) = delete;
	
	GraphicsContext();

	~GraphicsContext();

	void updateBuffer(BufferView* const bufferView, const void* const data, const uint32_t size);

	//per frame global resources transition immediate
	void setGlobalConstantBuffer(const IndexConstantBuffer* const indexBuffer);

	void setGlobalConstantBuffer(const ConstantBuffer* const constantBuffer);

	//4 values availiable
	void setVSConstants(const std::initializer_list<ShaderResourceDesc>& descs, const uint32_t offset);

	//4 values availiable
	void setVSConstants(const uint32_t numValues, const void* const data, const uint32_t offset);

	//4 values availiable
	void setHSConstants(const std::initializer_list<ShaderResourceDesc>& descs, const uint32_t offset);

	//4 values availiable
	void setHSConstants(const uint32_t numValues, const void* const data, const uint32_t offset);

	//8 values availiable
	void setDSConstants(const std::initializer_list<ShaderResourceDesc>& descs, const uint32_t offset);

	//8 values availiable
	void setDSConstants(const uint32_t numValues, const void* const data, const uint32_t offset);

	//4 values availiable
	void setGSConstants(const std::initializer_list<ShaderResourceDesc>& descs, const uint32_t offset);

	//4 values availiable
	void setGSConstants(const uint32_t numValues, const void* const data, const uint32_t offset);

	//24 values availiable
	void setPSConstants(const std::initializer_list<ShaderResourceDesc>& descs, const uint32_t offset);

	//24 values availiable
	void setPSConstants(const uint32_t numValues, const void* const data, const uint32_t offset);

	//32 values availiable
	void setCSConstants(const std::initializer_list<ShaderResourceDesc>& descs, const uint32_t offset);

	//32 values availiable
	void setCSConstants(const uint32_t numValues, const void* const data, const uint32_t offset);

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

	void setCSConstantBuffer(const IndexConstantBuffer* const constantBuffer);

	void setCSConstantBuffer(const ConstantBuffer* const constantBuffer);

	//must call this method after all resource binding methods
	void transitionResources();

	void setRenderTargets(const std::initializer_list<RenderTargetDesc>& renderTargets, const DepthStencilDesc* const depthStencils = nullptr);

	void setDefRenderTarget() const;

	void clearDefRenderTarget(const float clearValue[4]) const;

	void setVertexBuffers(const uint32_t startSlot, const std::initializer_list<VertexBufferDesc>& vertexBuffers);

	void setIndexBuffer(const IndexBufferDesc& indexBuffers);

	void setTopology(const D3D12_PRIMITIVE_TOPOLOGY topology) const;

	void setViewport(const float width, const float height);

	void setViewport(const uint32_t width, const uint32_t height);

	void setScissorRect(const uint32_t left, const uint32_t top, const uint32_t right, const uint32_t bottom);

	void setScissorRect(const float left, const float top, const float right, const float bottom);

	void setViewportSimple(const float width, const float height);

	void setViewportSimple(const uint32_t width, const uint32_t height);

	void setPipelineState(ID3D12PipelineState* const pipelineState) const;

	void clearRenderTarget(const RenderTargetDesc& desc, const float clearValue[4]) const;

	void clearDepthStencil(const DepthStencilDesc& desc, const D3D12_CLEAR_FLAGS flags, const float depth, const uint8_t stencil) const;

	void clearUnorderedAccess(const ClearUAVDesc& desc, const float values[4]);

	void clearUnorderedAccess(const ClearUAVDesc& desc, const uint32_t values[4]);

	void uavBarrier(const std::initializer_list<Resource*>& resources) const;

	void draw(const uint32_t vertexCountPerInstance, const uint32_t instanceCount, const uint32_t startVertexLocation, const uint32_t startInstanceLocation) const;

	void drawIndexed(const uint32_t indexCountPerInstance, const uint32_t instanceCount, const uint32_t startIndexLocation, const int baseVertexLocation, const uint32_t startInstanceLocation) const;

	void dispatch(const uint32_t threadGroupCountX, const uint32_t threadGroupCountY, const uint32_t threadGroupCountZ) const;

	void begin() const;

	void end() const;

	CommandList* getCommandList() const;

private:

	friend class RenderEngine;

	static ConstantBuffer* globalConstantBuffer;

	void getIndicesFromResourceDescs(const std::initializer_list<ShaderResourceDesc>& descs, uint32_t* const dst) const;

	D3D12_VIEWPORT vp;

	D3D12_RECT rt;

	CommandList* const commandList;

	uint32_t tempResourceIndices[32];

};


#endif // !_GRAPHICSCONTEXT_H_