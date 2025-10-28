#pragma once

#ifndef _CORE_GLOBALEFFECT_LATLONGMAPTOCUBEMAPEFFECT_H_
#define _CORE_GLOBALEFFECT_LATLONGMAPTOCUBEMAPEFFECT_H_

#include"GlobalEffectHeader.h"

namespace Core
{
	namespace GlobalEffect
	{
		namespace LatLongMapToCubeMapEffect
		{
			void process(GraphicsContext* const context, Resource::TextureRenderView* const inputTexture, Resource::TextureRenderView* const outputTexture);
		}
	}
}

#endif // !_CORE_GLOBALEFFECT_LATLONGMAPTOCUBEMAPEFFECT_H_
