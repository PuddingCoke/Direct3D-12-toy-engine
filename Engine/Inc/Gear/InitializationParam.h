#pragma once

#ifndef _GEAR_INITIALIZATIONPARAM_H_
#define _GEAR_INITIALIZATIONPARAM_H_

#include<string>

namespace Gear
{
	struct InitializationParam
	{
		//使用nsight调试前请关闭ImGui，否则程序会莫名崩溃
		static InitializationParam RealTime(const uint32_t width, const uint32_t height, const std::wstring& title, const bool enableImGuiSurface = true);

		//对于可循环动画来说，持续时间必须是2s的倍数
		static InitializationParam VideoRender(const uint32_t width, const uint32_t height, const uint32_t seconds = 60u, const uint32_t maxBFrames = 2);

		static InitializationParam Wallpaper();

		enum EngineUsage
		{
			REALTIMERENDER,//实时模式
			VIDEORENDER,//视频渲染模式
			WALLPAPER//动态壁纸模式
		}usage;

		std::wstring title;//窗口标题

		uint32_t width;//后备缓冲宽度，仅用于实时模式和视频渲染模式

		uint32_t height;//后备缓冲高度，仅用于实时模式和视频渲染模式

		bool enableImGuiSurface;//是否开启ImGui界面，仅用于实时模式

		//视频渲染模式配置
		struct VideoRenderParam
		{

			uint32_t seconds;//视频时间

			uint32_t maxBFrames;//最多连续B帧

			bool hardwareEncode;//是否使用硬件编码，若追求最高质量，请使用软件编码

		} videoRender;

	};
}

#endif // !_GEAR_INITIALIZATIONPARAM_H_
