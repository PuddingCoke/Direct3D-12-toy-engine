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
		VIDEOPLAYBACK
	} usage;

	const uint32_t width;

	const uint32_t height;

	const std::wstring title;

	const bool enableImGuiSurface;

	//disable imgui before using nsight otherwise program will crash somehow
	Configuration(const uint32_t width, const uint32_t height, const std::wstring& title, const EngineUsage usage = EngineUsage::NORMAL, const bool enableImGuiSurface = true);

};

#endif // !_CONFIGURATION_H_