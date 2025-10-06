﻿#pragma once

#ifndef _INITIALIZATION_H_
#define _INITIALIZATION_H_

#include<string>

struct InitializationParam
{

	//please disable imgui before using nsight otherwise program will crash
	static InitializationParam RealTime(const uint32_t width, const uint32_t height, const std::wstring& title, const bool enableImGuiSurface = true);

	static InitializationParam VideoRender(const uint32_t width, const uint32_t height, const uint32_t second = 60u);

	enum class EngineUsage
	{
		REALTIMERENDER,
		VIDEORENDER
	}usage;

	std::wstring title;

	struct RealTimeRenderParam
	{
		uint32_t width;

		uint32_t height;

		bool enableImGuiSurface;
	};

	struct VideoRenderParam
	{
		uint32_t width;

		uint32_t height;

		uint32_t second;
	};

	union
	{
		RealTimeRenderParam realTimeRender;

		VideoRenderParam videoRender;
	};

};

#endif // !_INITIALIZATION_H_