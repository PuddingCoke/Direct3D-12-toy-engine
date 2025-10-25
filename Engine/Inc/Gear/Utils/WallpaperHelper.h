#pragma once

#ifndef _UTILS_WALLPAPERHELPER_H_
#define _UTILS_WALLPAPERHELPER_H_

#include<cstdint>

#define NOMINMAX

#include<Windows.h>

namespace Utils
{
	namespace WallpaperHelper
	{

		void getSystemResolution(uint32_t& width, uint32_t& height);

		HWND getWallpaperHWND();

	}
}

#endif // !_UTILS_WALLPAPERHELPER_H_
