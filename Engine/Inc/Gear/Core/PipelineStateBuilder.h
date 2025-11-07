#pragma once

#ifndef _GEAR_CORE_PIPELINESTATEBUILDER_H_
#define _GEAR_CORE_PIPELINESTATEBUILDER_H_

#include<Gear/Core/D3D12Core/PipelineState.h>

#include<Gear/Core/D3D12Core/Shader.h>

namespace Gear
{
	namespace Core
	{
		class PipelineStateBuilder
		{
		public:

			PipelineStateBuilder();

			~PipelineStateBuilder();

			PipelineStateBuilder& setVS(const D3D12Core::Shader* const vs);

			PipelineStateBuilder& setHS(const D3D12Core::Shader* const hs);

			PipelineStateBuilder& setDS(const D3D12Core::Shader* const ds);

			PipelineStateBuilder& setGS(const D3D12Core::Shader* const gs);

			PipelineStateBuilder& setPS(const D3D12Core::Shader* const ps);

			template<size_t N>
			PipelineStateBuilder& setRTVFormats(const DXGI_FORMAT(&rtvFormats)[N]);

			PipelineStateBuilder& setNoRTV();

			PipelineStateBuilder& setDSVFormat(const DXGI_FORMAT format);

			template<size_t N>
			PipelineStateBuilder& setInputElements(const D3D12_INPUT_ELEMENT_DESC(&descs)[N]);

			PipelineStateBuilder& setPrimitiveTopologyType(const D3D12_PRIMITIVE_TOPOLOGY_TYPE primitiveTopologyType);

			PipelineStateBuilder& setBlendState(const D3D12_BLEND_DESC& desc);

			PipelineStateBuilder& setRasterizerState(const D3D12_RASTERIZER_DESC& desc);

			PipelineStateBuilder& setDepthStencilState(const D3D12_DEPTH_STENCIL_DESC& desc);

			PipelineStateBuilder& setDefaultFullScreenState();

			D3D12Core::PipelineState* build();

			static D3D12Core::PipelineState* buildComputeState(const D3D12Core::Shader* const shader);

		private:

			PipelineStateBuilder& setCS(const D3D12Core::Shader* const cs);

			std::vector<D3D12_INPUT_ELEMENT_DESC> inputElements;

			D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsDesc;

			const D3D12Core::Shader* vertexShader;

			const D3D12Core::Shader* hullShader;

			const D3D12Core::Shader* domainShader;

			const D3D12Core::Shader* geometryShader;

			const D3D12Core::Shader* pixelShader;

			const D3D12Core::Shader* computeShader;

		};

		template<size_t N>
		inline PipelineStateBuilder& PipelineStateBuilder::setRTVFormats(const DXGI_FORMAT(&rtvFormats)[N])
		{
			uint32_t rtvIndex = 0u;

			for (; rtvIndex < N; rtvIndex++)
			{
				graphicsDesc.RTVFormats[rtvIndex] = rtvFormats[rtvIndex];
			}

			graphicsDesc.NumRenderTargets = rtvIndex;

			return *this;
		}

		template<size_t N>
		inline PipelineStateBuilder& PipelineStateBuilder::setInputElements(const D3D12_INPUT_ELEMENT_DESC(&descs)[N])
		{
			for (uint32_t i = 0; i < N; i++)
			{
				inputElements.push_back(descs[i]);
			}

			graphicsDesc.InputLayout = { inputElements.data(),static_cast<uint32_t>(inputElements.size()) };

			return *this;
		}
	}
}

#endif // !_GEAR_CORE_PIPELINESTATEBUILDER_H_
