#pragma once

#ifndef _GRAPHICSCONTEXT_H_
#define _GRAPHICSCONTEXT_H_

#include<Gear/Core/CommandList.h>

#include<Gear/Core/Resource/BufferView.h>

#include<Gear/Core/Resource/StaticCBuffer.h>

class GraphicsContext
{
public:

	GraphicsContext(const GraphicsContext&) = delete;

	void operator=(const GraphicsContext&) = delete;

	GraphicsContext();

	~GraphicsContext();

	void updateBuffer(BufferView* const bufferView, const void* const data, const uint32_t size) const;

	void updateBuffer(StaticCBuffer* const staticCBuffer, const void* const data, const uint32_t size) const;

	//4 values availiable
	void setVSConstants(const std::initializer_list<ShaderResourceDesc>& descs, const uint32_t offset);

	//4 values availiable
	void setVSConstants(const uint32_t numValues, const void* const data, const uint32_t offset) const;

	//4 values availiable
	void setHSConstants(const std::initializer_list<ShaderResourceDesc>& descs, const uint32_t offset);

	//4 values availiable
	void setHSConstants(const uint32_t numValues, const void* const data, const uint32_t offset) const;

	//8 values availiable
	void setDSConstants(const std::initializer_list<ShaderResourceDesc>& descs, const uint32_t offset);

	//8 values availiable
	void setDSConstants(const uint32_t numValues, const void* const data, const uint32_t offset) const;

	//4 values availiable
	void setGSConstants(const std::initializer_list<ShaderResourceDesc>& descs, const uint32_t offset);

	//4 values availiable
	void setGSConstants(const uint32_t numValues, const void* const data, const uint32_t offset) const;

	//24 values availiable
	void setPSConstants(const std::initializer_list<ShaderResourceDesc>& descs, const uint32_t offset);

	//24 values availiable
	void setPSConstants(const uint32_t numValues, const void* const data, const uint32_t offset) const;

	//32 values availiable
	void setCSConstants(const std::initializer_list<ShaderResourceDesc>& descs, const uint32_t offset);

	//32 values availiable
	void setCSConstants(const uint32_t numValues, const void* const data, const uint32_t offset) const;

	void setGlobalConstantBuffer(const ImmutableCBuffer* const immutableCBuffer);

	void setVSConstantBuffer(const ImmutableCBuffer* const immutableCBuffer);

	void setHSConstantBuffer(const ImmutableCBuffer* const immutableCBuffer);

	void setDSConstantBuffer(const ImmutableCBuffer* const immutableCBuffer);

	void setGSConstantBuffer(const ImmutableCBuffer* const immutableCBuffer);

	void setPSConstantBuffer(const ImmutableCBuffer* const immutableCBuffer);

	void setCSConstantBuffer(const ImmutableCBuffer* const immutableCBuffer);

	//must call this method after all resource binding methods
	void transitionResources();

	void setRenderTargets(const std::initializer_list<RenderTargetDesc>& renderTargets, const DepthStencilDesc* const depthStencils = nullptr) const;

	void setDefRenderTarget() const;

	void clearDefRenderTarget(const float clearValue[4]) const;

	void setVertexBuffers(const uint32_t startSlot, const std::initializer_list<VertexBufferDesc>& vertexBuffers) const;

	void setIndexBuffer(const IndexBufferDesc& indexBuffers) const;

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

	CommandList* getCommandList() const;

private:

	friend class RenderEngine;

	static ImmutableCBuffer* reservedGlobalCBuffer;
	
	struct RootConstantBufferDesc
	{
		enum Type
		{
			GRAPHICS, COMPUTE
		} type;

		uint32_t rootParameterIndex;

		D3D12_GPU_VIRTUAL_ADDRESS gpuAddress;
	};

	void getResourceIndicesFromDescs(const std::initializer_list<ShaderResourceDesc>& descs);

	void pushRootConstantBufferDesc(const RootConstantBufferDesc& desc);

	D3D12_VIEWPORT vp;

	D3D12_RECT rt;

	CommandList* const commandList;

	uint32_t resourceIndices[32];

	std::vector<RootConstantBufferDesc> rootConstantBufferDescs;

};


#endif // !_GRAPHICSCONTEXT_H_