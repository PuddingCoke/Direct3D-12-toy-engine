#pragma once

#ifndef _BACKBUFFERBLITEFFECT_H_
#define _BACKBUFFERBLITEFFECT_H_

#include"StaticEffectHeader.h"

//draw texture to back buffer
class BackBufferBlitEffect
{
public:

	BackBufferBlitEffect(const BackBufferBlitEffect&) = delete;

	void operator=(const BackBufferBlitEffect&) = delete;

	static BackBufferBlitEffect* get();

	void process(GraphicsContext* const context, TextureRenderView* const inputTexture);

private:

	static friend class StaticResourceManager;

	static BackBufferBlitEffect* instance;

	BackBufferBlitEffect();

	~BackBufferBlitEffect();

	ComPtr<ID3D12PipelineState> backBufferBlitState;

};

#endif // !_BACKBUFFERBLITEFFECT_H_
