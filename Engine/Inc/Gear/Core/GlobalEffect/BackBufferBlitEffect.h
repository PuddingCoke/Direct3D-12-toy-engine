#pragma once

#ifndef _CORE_GLOBALEFFECT_BACKBUFFERBLITEFFECT_H_
#define _CORE_GLOBALEFFECT_BACKBUFFERBLITEFFECT_H_

#include"GlobalEffectHeader.h"

namespace Core
{
	namespace GlobalEffect
	{
		namespace BackBufferBlitEffect
		{
			void process(GraphicsContext* const context, Resource::TextureRenderView* const inputTexture);
		}
	}
}

#endif // !_CORE_GLOBALEFFECT_BACKBUFFERBLITEFFECT_H_
