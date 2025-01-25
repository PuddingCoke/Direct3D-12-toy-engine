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

	static void createComputeState(ID3D12PipelineState** pipelineState, const Shader* const shader);

};

#endif // !_PIPELINESTATE_H_