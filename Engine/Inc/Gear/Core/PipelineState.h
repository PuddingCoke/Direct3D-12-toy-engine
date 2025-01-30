#pragma once

#ifndef _PIPELINESTATE_H_
#define _PIPELINESTATE_H_

#include<Gear/Core/Shader.h>

//defined some useful state
class PipelineState
{
public:

	PipelineState() = delete;

	PipelineState(const PipelineState&) = delete;

	void operator=(const PipelineState&) = delete;

	static D3D12_BLEND_DESC defBlendDesc;

	static D3D12_BLEND_DESC addtiveBlendDesc;

	static D3D12_RASTERIZER_DESC rasterShadow;

	static D3D12_RASTERIZER_DESC rasterCullBack;

	static D3D12_RASTERIZER_DESC rasterCullFront;

	static D3D12_RASTERIZER_DESC rasterCullNone;

	static D3D12_RASTERIZER_DESC rasterWireFrame;

	static D3D12_DEPTH_STENCIL_DESC depthLessEqual;

	static D3D12_DEPTH_STENCIL_DESC depthLess;

	//configure
	//InputLayout
	//BlendState
	//RasterizerState
	//DepthStencilState
	//PrimitiveTopologyType
	//NumRenderTargets
	//RTVFormats
	//DSVFormat
	static D3D12_GRAPHICS_PIPELINE_STATE_DESC getDefaultGraphicsDesc();

	//configure
	//NumRenderTargets
	//RTVFormats
	//PS
	static D3D12_GRAPHICS_PIPELINE_STATE_DESC getDefaultFullScreenState();

	static void createComputeState(ID3D12PipelineState** const pipelineState, const Shader* const shader);

private:

	friend class RenderEngine;

	static void initialize();

};

#endif // !_PIPELINESTATE_H_