#pragma once

#ifndef _GEAR_CORE_GRAPHICSCONTEXT_H_
#define _GEAR_CORE_GRAPHICSCONTEXT_H_

#include<Gear/Core/D3D12Core/CommandList.h>

#include<Gear/Core/Resource/BufferView.h>

#include<Gear/Core/Resource/StaticCBuffer.h>

namespace Gear
{
	namespace Core
	{
		class GraphicsContext
		{
		public:

			GraphicsContext(const GraphicsContext&) = delete;

			void operator=(const GraphicsContext&) = delete;

			GraphicsContext();

			~GraphicsContext();

			void updateBuffer(Resource::BufferView* const bufferView, const void* const data, const uint32_t size) const;

			void updateBuffer(Resource::StaticCBuffer* const staticCBuffer, const void* const data, const uint32_t size) const;

			//4个值可用
			void setVSConstants(const std::initializer_list<Resource::D3D12Resource::ShaderResourceDesc>& descs, const uint32_t offset);

			//4个值可用
			void setVSConstants(const uint32_t numValues, const void* const data, const uint32_t offset) const;

			//4个值可用
			void setHSConstants(const std::initializer_list<Resource::D3D12Resource::ShaderResourceDesc>& descs, const uint32_t offset);

			//4个值可用
			void setHSConstants(const uint32_t numValues, const void* const data, const uint32_t offset) const;

			//8个值可用
			void setDSConstants(const std::initializer_list<Resource::D3D12Resource::ShaderResourceDesc>& descs, const uint32_t offset);

			//8个值可用
			void setDSConstants(const uint32_t numValues, const void* const data, const uint32_t offset) const;

			//4个值可用
			void setGSConstants(const std::initializer_list<Resource::D3D12Resource::ShaderResourceDesc>& descs, const uint32_t offset);

			//4个值可用
			void setGSConstants(const uint32_t numValues, const void* const data, const uint32_t offset) const;

			//24个值可用
			void setPSConstants(const std::initializer_list<Resource::D3D12Resource::ShaderResourceDesc>& descs, const uint32_t offset);

			//24个值可用
			void setPSConstants(const uint32_t numValues, const void* const data, const uint32_t offset) const;

			//32个值可用
			void setCSConstants(const std::initializer_list<Resource::D3D12Resource::ShaderResourceDesc>& descs, const uint32_t offset);

			//32个值可用
			void setCSConstants(const uint32_t numValues, const void* const data, const uint32_t offset) const;

			void setGlobalConstantBuffer(const Resource::ImmutableCBuffer* const immutableCBuffer);

			void setVSConstantBuffer(const Resource::ImmutableCBuffer* const immutableCBuffer);

			void setHSConstantBuffer(const Resource::ImmutableCBuffer* const immutableCBuffer);

			void setDSConstantBuffer(const Resource::ImmutableCBuffer* const immutableCBuffer);

			void setGSConstantBuffer(const Resource::ImmutableCBuffer* const immutableCBuffer);

			void setPSConstantBuffer(const Resource::ImmutableCBuffer* const immutableCBuffer);

			void setCSConstantBuffer(const Resource::ImmutableCBuffer* const immutableCBuffer);

			//绑定资源后必须调用这个方法！！
			void transitionResources();

			void setRenderTargets(const std::initializer_list<Resource::D3D12Resource::RenderTargetDesc>& renderTargets, const Resource::D3D12Resource::DepthStencilDesc* const depthStencils = nullptr) const;

			void setDefRenderTarget() const;

			void clearDefRenderTarget(const float clearValue[4]) const;

			void setVertexBuffers(const uint32_t startSlot, const std::initializer_list<Resource::D3D12Resource::VertexBufferDesc>& vertexBuffers) const;

			void setIndexBuffer(const Resource::D3D12Resource::IndexBufferDesc& indexBuffers) const;

			void setPrimitiveTopology(const D3D12_PRIMITIVE_TOPOLOGY topology) const;

			void setViewport(const float width, const float height);

			void setViewport(const uint32_t width, const uint32_t height);

			void setScissorRect(const uint32_t left, const uint32_t top, const uint32_t right, const uint32_t bottom);

			void setScissorRect(const float left, const float top, const float right, const float bottom);

			void setViewportSimple(const float width, const float height);

			void setViewportSimple(const uint32_t width, const uint32_t height);

			void setPipelineState(ID3D12PipelineState* const pipelineState) const;

			void clearRenderTarget(const Resource::D3D12Resource::RenderTargetDesc& desc, const float clearValue[4]) const;

			void clearDepthStencil(const Resource::D3D12Resource::DepthStencilDesc& desc, const D3D12_CLEAR_FLAGS flags, const float depth, const uint8_t stencil) const;

			void clearUnorderedAccess(const Resource::D3D12Resource::ClearUAVDesc& desc, const float values[4]);

			void clearUnorderedAccess(const Resource::D3D12Resource::ClearUAVDesc& desc, const uint32_t values[4]);

			void uavBarrier(const std::initializer_list<Resource::D3D12Resource::D3D12ResourceBase*>& resources) const;

			void draw(const uint32_t vertexCountPerInstance, const uint32_t instanceCount, const uint32_t startVertexLocation, const uint32_t startInstanceLocation) const;

			void drawIndexed(const uint32_t indexCountPerInstance, const uint32_t instanceCount, const uint32_t startIndexLocation, const int32_t baseVertexLocation, const uint32_t startInstanceLocation) const;

			void dispatch(const uint32_t threadGroupCountX, const uint32_t threadGroupCountY, const uint32_t threadGroupCountZ) const;

			void begin() const;

			D3D12Core::CommandList* getCommandList() const;

		private:

			struct RootConstantBufferDesc
			{
				enum Type
				{
					GRAPHICS, COMPUTE
				} type;

				uint32_t rootParameterIndex;

				D3D12_GPU_VIRTUAL_ADDRESS gpuAddress;
			};

			void getResourceIndicesFromDescs(const std::initializer_list<Resource::D3D12Resource::ShaderResourceDesc>& descs);

			void pushRootConstantBufferDesc(const RootConstantBufferDesc& desc);

			D3D12_VIEWPORT vp;

			D3D12_RECT rt;

			D3D12Core::CommandList* const commandList;

			uint32_t resourceIndices[32];

			std::vector<RootConstantBufferDesc> rootConstantBufferDescs;

		};
	}
}

#endif // !_GEAR_CORE_GRAPHICSCONTEXT_H_