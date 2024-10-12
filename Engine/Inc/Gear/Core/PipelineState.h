#pragma once

#ifndef _PIPELINESTATE_H_
#define _PIPELINESTATE_H_

#include<Gear/Core/States.h>
#include<Gear/Core/Shader.h>
#include<Gear/Core/Graphics.h>
#include<Gear/Core/GraphicsDevice.h>
#include<Gear/Core/GlobalRootSignature.h>

#include<Gear/CompiledShaders/EquirectangularVS.h>
#include<Gear/CompiledShaders/EquirectangularPS.h>

//defined some useful state
class PipelineState
{
public:

	ComPtr<ID3D12PipelineState> equirectangularR8State;

	ComPtr<ID3D12PipelineState> equirectangularR16State;

	//simply draw texture to backbuffer
	ComPtr<ID3D12PipelineState> fullScreenBlitState;

	static PipelineState* get();

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

	static ID3D12PipelineState* createComputeState(const Shader* const shader);

private:

	friend class RenderEngine;

	PipelineState();

	~PipelineState();

	static PipelineState* instance;

};

#endif // !_PIPELINESTATE_H_
