#pragma once

#ifndef _CORE_STATICEFFECT_BACKBUFFERBLITEFFECT_H_
#define _CORE_STATICEFFECT_BACKBUFFERBLITEFFECT_H_

#include"StaticEffectHeader.h"

namespace Core
{
	namespace StaticEffect
	{
		namespace BackBufferBlitEffect
		{
			void process(GraphicsContext* const context, TextureRenderView* const inputTexture);
		}
	}
}

#endif // !_CORE_STATICEFFECT_BACKBUFFERBLITEFFECT_H_
