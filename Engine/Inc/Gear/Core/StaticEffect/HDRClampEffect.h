#pragma once

#ifndef _HDRCLAMPEFFECT_H_
#define _HDRCLAMPEFFECT_H_

#include"StaticEffectHeader.h"

//it seems OpenEXR api won't handle nan or nif case when cast float to half
//this effect require a compute texture input and then reset its nan and inf texels
class HDRClampEffect
{
public:

	HDRClampEffect(const HDRClampEffect&) = delete;

	void operator=(const HDRClampEffect&) = delete;

	static HDRClampEffect* get();

	void process(GraphicsContext* const context, TextureRenderView* const inOutTexture) const;

private:

	friend class StaticResourceManager;

	static HDRClampEffect* instance;

	HDRClampEffect();

	~HDRClampEffect();

	Shader* const hdrClampShader;

	ComPtr<ID3D12PipelineState> hdrClampState;

};

#endif // !_HDRCLAMPEFFECT_H_
