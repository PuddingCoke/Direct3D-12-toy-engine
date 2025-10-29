#pragma once

#ifndef _CORE_RESOURCE_D3D12RESOURCE_PIPELINERESOURCEDESC_H_
#define _CORE_RESOURCE_D3D12RESOURCE_PIPELINERESOURCEDESC_H_

#include<D3D12Headers/d3dx12.h>

namespace Core
{
	namespace Resource
	{
		namespace D3D12Resource
		{
			class Buffer;

			class Texture;

			struct ShaderResourceDesc
			{
				enum ResourceType
				{
					BUFFER,
					TEXTURE
				} type;

				enum TargetStates
				{
					SRV,
					UAV,
					CBV
				} state;

				uint32_t resourceIndex;

				struct TextureTransitionDesc
				{
					D3D12Resource::Texture* texture;
					uint32_t mipSlice;
				};

				struct BufferTransitionDesc
				{
					D3D12Resource::Buffer* buffer;
					D3D12Resource::Buffer* counterBuffer;
				};

				union
				{
					TextureTransitionDesc textureDesc;
					BufferTransitionDesc bufferDesc;
				};
			};

			struct RenderTargetDesc
			{
				D3D12Resource::Texture* texture;
				uint32_t mipSlice;
				D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle;
			};

			struct DepthStencilDesc
			{
				D3D12Resource::Texture* texture;
				uint32_t mipSlice;
				D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle;
			};

			struct VertexBufferDesc
			{
				D3D12Resource::Buffer* buffer;
				D3D12_VERTEX_BUFFER_VIEW vbv;
			};

			struct IndexBufferDesc
			{
				D3D12Resource::Buffer* buffer;
				D3D12_INDEX_BUFFER_VIEW ibv;
			};

			struct ClearUAVDesc
			{
				enum ResourceType
				{
					BUFFER,
					TEXTURE
				} type;

				struct TextureClearDesc
				{
					D3D12Resource::Texture* texture;
					uint32_t mipSlice;
				};

				struct BufferClearDesc
				{
					D3D12Resource::Buffer* buffer;
				};

				union
				{
					TextureClearDesc textureDesc;
					BufferClearDesc bufferDesc;
				};

				D3D12_GPU_DESCRIPTOR_HANDLE viewGPUHandle;

				D3D12_CPU_DESCRIPTOR_HANDLE viewCPUHandle;
			};
		}
	}
}

#endif // !_CORE_RESOURCE_D3D12RESOURCE_PIPELINERESOURCEDESC_H_
