#pragma once

#ifndef _CORE_STATICEFFECT_HDRCLAMPEFFECT_H_
#define _CORE_STATICEFFECT_HDRCLAMPEFFECT_H_

#include"StaticEffectHeader.h"

namespace Core
{
	namespace StaticEffect
	{
		namespace HDRClampEffect
		{
			void process(GraphicsContext* const context, TextureRenderView* const inOutTexture);
		}
	}
}

#endif // !_CORE_STATICEFFECT_HDRCLAMPEFFECT_H_
