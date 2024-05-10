#pragma once

#ifndef _EFFECT_H_
#define _EFFECT_H_

#include<Gear/Core/GraphicsContext.h>

#include<Gear/Core/ResourceManager.h>

#include<ImGUI/imgui.h>

class Effect
{
public:

	Effect() = delete;

	Effect(const Effect&) = delete;

	void operator=(const Effect&) = delete;

	Effect(GraphicsContext* const context, const UINT width, const UINT height, const DXGI_FORMAT format);

	virtual ~Effect();

	virtual void imGUICall() = 0;

protected:

	TextureRenderView* outputTexture;

	GraphicsContext* context;

	const UINT width;

	const UINT height;

};

#endif // !_EFFECT_H_
