﻿#pragma once

#ifndef _GEAR_CORE_GLOBALEFFECT_LATLONGMAPTOCUBEMAPEFFECT_INTERNAL_H_
#define _GEAR_CORE_GLOBALEFFECT_LATLONGMAPTOCUBEMAPEFFECT_INTERNAL_H_

#include<Gear/Core/ResourceManager.h>

namespace Gear
{
	namespace Core
	{
		namespace GlobalEffect
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
}

#endif // !_GEAR_CORE_GLOBALEFFECT_LATLONGMAPTOCUBEMAPEFFECT_INTERNAL_H_
