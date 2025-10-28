#pragma once

#ifndef _CORE_EFFECT_EFFECTBASE_H_
#define _CORE_EFFECT_EFFECTBASE_H_

#include<Gear/Core/ResourceManager.h>

#include<Gear/Core/D3D12Core/Shader.h>

#include<Gear/Core/PipelineStateHelper.h>

#include<ImGUI/imgui.h>

namespace Core
{
	namespace Effect
	{
		class EffectBase
		{
		public:

			EffectBase() = delete;

			EffectBase(const EffectBase&) = delete;

			void operator=(const EffectBase&) = delete;

			EffectBase(GraphicsContext* const context, const uint32_t width, const uint32_t height, const DXGI_FORMAT format);

			virtual ~EffectBase();

			virtual void imGUICall() = 0;

		protected:

			Resource::TextureRenderView* const outputTexture;

			GraphicsContext* const context;

			const uint32_t width;

			const uint32_t height;

		};
	}
}

#endif // !_CORE_EFFECT_EFFECTBASE_H_