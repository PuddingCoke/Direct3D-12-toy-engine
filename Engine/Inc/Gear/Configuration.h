#pragma once

#ifndef _CONFIGURATION_H_
#define _CONFIGURATION_H_

#include<string>

class Configuration
{
public:

	const enum class EngineUsage
	{
		NORMAL,
		WALLPAPER,
		VIDEOPLAYBACK
	} usage;

	const int width;

	const int height;

	const std::wstring title;

	Configuration(const int& width, const int& height, const std::wstring& title, const EngineUsage& usage = EngineUsage::NORMAL);

};

#endif // !_CONFIGURATION_H_
