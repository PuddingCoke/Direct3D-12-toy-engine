#pragma once

#ifndef _CORE_GLOBALEFFECT_HDRCLAMPEFFECT_H_
#define _CORE_GLOBALEFFECT_HDRCLAMPEFFECT_H_

#include"GlobalEffectHeader.h"

namespace Core
{
	namespace GlobalEffect
	{
		namespace HDRClampEffect
		{
			void process(GraphicsContext* const context, TextureRenderView* const inOutTexture);
		}
	}
}

#endif // !_CORE_GLOBALCEFFECT_HDRCLAMPEFFECT_H_
