#pragma once

#ifndef _CORE_STATICEFFECT_LATLONGMAPTOCUBEMAPEFFECT_H_
#define _CORE_STATICEFFECT_LATLONGMAPTOCUBEMAPEFFECT_H_

#include"StaticEffectHeader.h"

namespace Core
{
	namespace StaticEffect
	{
		namespace LatLongMapToCubeMapEffect
		{
			void process(GraphicsContext* const context, TextureRenderView* const inputTexture, TextureRenderView* const outputTexture);
		}
	}
}

#endif // !_CORE_STATICEFFECT_LATLONGMAPTOCUBEMAPEFFECT_H_
