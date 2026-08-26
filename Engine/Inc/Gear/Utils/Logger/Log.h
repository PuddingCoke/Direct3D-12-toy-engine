#pragma once

#ifndef _GEAR_UTILS_LOGGER_LOG_H_
#define _GEAR_UTILS_LOGGER_LOG_H_

#include"LogContext.h"

using LogIntegerMode = Gear::Utils::Logger::LogContext::IntegerMode;

using LogFloatPrecision = Gear::Utils::Logger::LogContext::FloatPrecision;

using LogColor = Gear::Utils::Logger::LogColor;

using LogNewLine = Gear::Utils::Logger::LogContext::NewLine;

namespace Gear::Utils::Logger
{
	class Log
	{
	public:

		Log(const std::string_view& functionName, const LogType& type);

		~Log();

		//宽字符串
		const Log& operator<<(const std::wstring& arg) const;

		//原生宽字符串
		const Log& operator<<(const wchar_t* arg) const;

		//窄字符串
		const Log& operator<<(const std::string& arg) const;

		//原生窄字符串
		const Log& operator<<(const char* arg) const;

		//UTF8字符串
		const Log& operator<<(const std::u8string& arg) const;

		//原生UTF8字符串
		const Log& operator<<(const char8_t* arg) const;

		//布尔类型
		const Log& operator<<(const bool& arg) const;

		//有符号32位整数
		const Log& operator<<(const int32_t& arg) const;

		//有符号64位整数
		const Log& operator<<(const int64_t& arg) const;

		//无符号32位整数
		const Log& operator<<(const uint32_t& arg) const;

		//无符号64位整数
		const Log& operator<<(const uint64_t& arg) const;

		//浮点数
		const Log& operator<<(const float_t& arg) const;

		//双精度浮点数
		const Log& operator<<(const double_t& arg) const;

		//改变整数模式
		const Log& operator<<(const LogIntegerMode& arg) const;

		//改变浮点精度
		const Log& operator<<(const LogFloatPrecision& arg) const;

		//改变正文颜色
		const Log& operator<<(const LogColor& arg) const;

		void finishAndThrowMessage() const;

	private:

		bool needThrow() const;

		void finishAndPostMessage() const;

		LogContext& context;

	};

	void ThrowLog(const Log& log);

}

#endif // !_GEAR_UTILS_LOGGER_LOG_H_
