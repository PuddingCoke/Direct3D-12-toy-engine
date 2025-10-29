﻿#pragma once

#ifndef _GEAR_UTILS_WALLPAPERHELPER_H_
#define _GEAR_UTILS_WALLPAPERHELPER_H_

#include<cstdint>

#define NOMINMAX

#include<Windows.h>

namespace Gear
{
	namespace Utils
	{
		namespace WallpaperHelper
		{

			void getSystemResolution(uint32_t& width, uint32_t& height);

			HWND getWallpaperHWND();

		}
	}
}

#endif // !_GEAR_UTILS_WALLPAPERHELPER_H_
