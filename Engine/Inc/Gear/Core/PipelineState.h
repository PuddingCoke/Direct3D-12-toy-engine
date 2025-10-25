#pragma once

#ifndef _PIPELINESTATE_H_
#define _PIPELINESTATE_H_

#include<Gear/Core/Shader.h>

//helper namespace
namespace PipelineState
{

	constexpr D3D12_BLEND_DESC blendDefault = { false,false,
		{true,false,
		D3D12_BLEND_SRC_ALPHA,D3D12_BLEND_INV_SRC_ALPHA,D3D12_BLEND_OP_ADD,
		D3D12_BLEND_ONE,D3D12_BLEND_INV_SRC_ALPHA,D3D12_BLEND_OP_ADD,
		D3D12_LOGIC_OP_NOOP,D3D12_COLOR_WRITE_ENABLE_ALL
	} };

	constexpr D3D12_BLEND_DESC blendAddtive = { false,false,
		{true,false,
		D3D12_BLEND_ONE,D3D12_BLEND_ONE,D3D12_BLEND_OP_ADD,
		D3D12_BLEND_ONE,D3D12_BLEND_ZERO,D3D12_BLEND_OP_ADD,
		D3D12_LOGIC_OP_NOOP,D3D12_COLOR_WRITE_ENABLE_ALL
	} };

	constexpr D3D12_BLEND_DESC blendNone = { false,false,{false,false} };

	constexpr D3D12_RASTERIZER_DESC rasterShadow = { D3D12_FILL_MODE_SOLID,D3D12_CULL_MODE_BACK,false,16,0.f,4.f,true };

	constexpr D3D12_RASTERIZER_DESC rasterCullBack = { D3D12_FILL_MODE_SOLID,D3D12_CULL_MODE_BACK,false,0,0.f,0.f,true };

	constexpr D3D12_RASTERIZER_DESC rasterCullFront = { D3D12_FILL_MODE_SOLID,D3D12_CULL_MODE_FRONT,false,0,0.f,0.f,true };

	constexpr D3D12_RASTERIZER_DESC rasterCullNone = { D3D12_FILL_MODE_SOLID,D3D12_CULL_MODE_NONE,false,0,0.f,0.f,true };

	constexpr D3D12_RASTERIZER_DESC rasterWireFrame = { D3D12_FILL_MODE_WIREFRAME,D3D12_CULL_MODE_BACK,false,0,0.f,0.f,true };

	constexpr D3D12_DEPTH_STENCIL_DESC depthLessEqual = { true,D3D12_DEPTH_WRITE_MASK_ALL,D3D12_COMPARISON_FUNC_LESS_EQUAL,false };

	constexpr D3D12_DEPTH_STENCIL_DESC depthLess = { true,D3D12_DEPTH_WRITE_MASK_ALL,D3D12_COMPARISON_FUNC_LESS,false };

	//configure
	//InputLayout
	//BlendState
	//RasterizerState
	//DepthStencilState
	//PrimitiveTopologyType
	//NumRenderTargets
	//RTVFormats
	//DSVFormat
	D3D12_GRAPHICS_PIPELINE_STATE_DESC getDefaultGraphicsDesc();

	//configure
	//NumRenderTargets
	//RTVFormats
	//PS
	D3D12_GRAPHICS_PIPELINE_STATE_DESC getDefaultFullScreenState();

	void createComputeState(ID3D12PipelineState** const pipelineState, const Shader* const shader);

}

#endif // !_PIPELINESTATE_H_