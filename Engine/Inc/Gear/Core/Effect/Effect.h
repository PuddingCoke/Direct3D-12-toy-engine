﻿#pragma once

#ifndef _EFFECT_H_
#define _EFFECT_H_

#include<Gear/Core/ResourceManager.h>

#include<Gear/Core/Shader.h>

#include<Gear/Core/PipelineState.h>

#include<ImGUI/imgui.h>

class Effect
{
public:

	Effect() = delete;

	Effect(const Effect&) = delete;

	void operator=(const Effect&) = delete;

	Effect(GraphicsContext* const context, const uint32_t width, const uint32_t height, const DXGI_FORMAT format);

	virtual ~Effect();

	virtual void imGUICall() = 0;

protected:

	TextureRenderView* const outputTexture;

	GraphicsContext* const context;

	const uint32_t width;

	const uint32_t height;

};

#endif // !_EFFECT_H_