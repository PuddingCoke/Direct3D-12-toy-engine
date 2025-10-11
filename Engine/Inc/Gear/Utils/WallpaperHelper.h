#pragma once

#ifndef _WALLPAPERHELPER_H_
#define _WALLPAPERHELPER_H_

#include<cstdint>

#define NOMINMAX

#include<Windows.h>

namespace WallpaperHelper
{

	void getSystemResolution(uint32_t& width, uint32_t& height);

	HWND getWallpaperHWND();

}

#endif // !_WALLPAPERHELPER_H_
