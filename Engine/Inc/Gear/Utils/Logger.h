#pragma once

#ifndef _LOGGER_H_
#define _LOGGER_H_

#include<iostream>

#include<fstream>

#include"Logger/LogContext.h"

using IntegerMode = LogContext::IntegerMode;

using FloatPrecision = LogContext::FloatPrecision;

/// <summary>
/// a simple logger that output text with colors depend on different situations
/// numeric value like int32_t,uint32_t,float_t have special color
/// since i want to use utf-8 encoding so std::string and native string are not supported
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
/// you can use FloatPrecision to adjust fraction part's digit
/// such as LOGUSER(L"浮点测试", FloatPrecision(4), 125.6f, FloatPrecision(2), 125.7);
/// </summary>
class Logger
{
public:

	Logger(const Logger&) = delete;

	void operator=(const Logger&) = delete;

	static Logger* get();

	void submitLogMessage(const LogMessage& msg);

private:

	friend class Gear;

	Logger();

	~Logger();

	static Logger* instance;

	std::wofstream file;

};

#define LOGSUCCESS(...) Logger::get()->submitLogMessage(LogContext::createLogMessage(wrapClassName(typeid(*this).name()),LogType::LOG_SUCCESS,__VA_ARGS__))

#define LOGENGINE(...) Logger::get()->submitLogMessage(LogContext::createLogMessage(wrapClassName(typeid(*this).name()),LogType::LOG_ENGINE,__VA_ARGS__))

#define LOGUSER(...) Logger::get()->submitLogMessage(LogContext::createLogMessage(wrapClassName(typeid(*this).name()),LogType::LOG_USER,__VA_ARGS__))

#define LOGERROR(...) Logger::get()->submitLogMessage(LogContext::createLogMessage(L"",LogType::LOG_ERROR,__FILEW__,__FUNCTIONW__,L"LINE",(int32_t)__LINE__,__VA_ARGS__)); \
throw std::runtime_error("check log.txt or console output for detailed information") \

#endif // !_LOGGER_H_