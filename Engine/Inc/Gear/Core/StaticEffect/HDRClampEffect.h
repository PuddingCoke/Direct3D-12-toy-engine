#pragma once

#ifndef _HDRCLAMPEFFECT_H_
#define _HDRCLAMPEFFECT_H_

#include<Gear/Core/GraphicsContext.h>

#include<Gear/Core/Shader.h>

#include<Gear/Core/PipelineState.h>

#include<Gear/Core/Resource/TextureRenderView.h>

#include<Gear/CompiledShaders/HDRClampCS.h>

//it seems OpenEXR api won't handle nan or nif case when cast float to half
//this effect require a compute texture input and then reset its nan and inf texels
class HDRClampEffect
{
public:

	static HDRClampEffect* get();

	void process(GraphicsContext* const context, TextureRenderView* const inOutTexture);

private:

	friend class StaticEffect;

	static HDRClampEffect* instance;

	static void initialize();

	static void release();

	HDRClampEffect();

	~HDRClampEffect();

	Shader* hdrClampShader;

	ComPtr<ID3D12PipelineState> hdrClampState;

};

#endif // !_HDRCLAMPEFFECT_H_
