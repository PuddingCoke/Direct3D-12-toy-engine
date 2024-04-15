#pragma once

#ifndef _PIPELINESTATE_H_
#define _PIPELINESTATE_H_

#include<Gear/Core/States.h>
#include<Gear/Core/Shader.h>
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

	static PipelineState* get();

private:

	friend class RenderEngine;

	PipelineState();

	~PipelineState();

	static PipelineState* instance;

};

#endif // !_PIPELINESTATE_H_
