#pragma once

#ifndef _UTILS_LOGGER_H_
#define _UTILS_LOGGER_H_

#include"Logger/LogContext.h"

using IntegerMode = Utils::Logger::LogContext::IntegerMode;

using FloatPrecision = Utils::Logger::LogContext::FloatPrecision;

using LogColor = Utils::Logger::LogColor;

/// <summary>
/// a simple logger that output text with colors depend on different situations
/// numeric value like int32_t,uint32_t,float_t have special color
/// due to implementation of LogContext std::string and native narrow string are not supported
/// and do not use logger in loop although there is optimization
/// 
/// following are supported format
/// integer types are int32_t,int64_t,uint32_t,uint64_t
/// float point types are float_t,double_t
/// string types are literal wchar_t array,std::wstring,const wchar_t*
/// 
/// you can use IntegerType to specify integer's output form
/// such as LOGUSER(L"32位无符号整数测试", IntegerMode::HEX, 13689u, UINT_MAX, IntegerMode::DEC, 13689u, UINT_MAX);
/// 
/// you can use FloatPrecision to adjust number of fraction part's digit
/// such as LOGUSER(L"浮点测试", FloatPrecision(4), 125.6f, FloatPrecision(2), 125.7);
/// 
/// there are many available colors in LogColor class
/// </summary>
namespace Utils
{
	namespace Logger
	{
		void submitLogMessage(const LogMessage& msg);
	}
}

#define LOGSUCCESS(...) Utils::Logger::submitLogMessage(Utils::Logger::LogContext::createLogMessage(__FUNCTIONW__,Utils::Logger::LogType::LOG_SUCCESS,__VA_ARGS__))

#define LOGENGINE(...) Utils::Logger::submitLogMessage(Utils::Logger::LogContext::createLogMessage(__FUNCTIONW__,Utils::Logger::LogType::LOG_ENGINE,__VA_ARGS__))

#define LOGUSER(...) Utils::Logger::submitLogMessage(Utils::Logger::LogContext::createLogMessage(__FUNCTIONW__,Utils::Logger::LogType::LOG_USER,__VA_ARGS__))

#define LOGERROR(...) Utils::Logger::submitLogMessage(Utils::Logger::LogContext::createLogMessage(__FUNCTIONW__,Utils::Logger::LogType::LOG_ERROR,__FILEW__,L"LINE",static_cast<int32_t>(__LINE__),__VA_ARGS__)); \
throw std::runtime_error("check log.txt or console output for detailed information") \

#endif // !_UTILS_LOGGER_H_