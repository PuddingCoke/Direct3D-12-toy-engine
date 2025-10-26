#pragma once

#ifndef _CORE_STATICEFFECT_LATLONGMAPTOCUBEMAPEFFECT_INTERNAL_H_
#define _CORE_STATICEFFECT_LATLONGMAPTOCUBEMAPEFFECT_INTERNAL_H_

#include<Gear/Core/ResourceManager.h>

namespace Core
{
	namespace StaticEffect
	{
		namespace LatLongMapToCubeMapEffect
		{
			namespace Internal
			{
				void initialize(ResourceManager* const resManager);

				void release();
			}
		}
	}
}

#endif // !_CORE_STATICEFFECT_LATLONGMAPTOCUBEMAPEFFECT_INTERNAL_H_
