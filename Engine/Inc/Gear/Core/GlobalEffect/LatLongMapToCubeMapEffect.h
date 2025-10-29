#pragma once

#ifndef _GEAR_CORE_GLOBALEFFECT_LATLONGMAPTOCUBEMAPEFFECT_H_
#define _GEAR_CORE_GLOBALEFFECT_LATLONGMAPTOCUBEMAPEFFECT_H_

#include"GlobalEffectHeader.h"

namespace Gear
{
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
}

#endif // !_GEAR_CORE_GLOBALEFFECT_LATLONGMAPTOCUBEMAPEFFECT_H_
