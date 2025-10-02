#pragma once

#ifndef _LOGGER_H_
#define _LOGGER_H_

#include<iostream>

#include<fstream>

#include"LogContext.h"

/// <summary>
/// a simple logger that output text with colors depend on different situations
/// and numeric value like int32_t uint32_t float_t have special color
/// since i want to use utf-8 encoding so std::string is not supported
/// do not use logger in loop although there is optimization
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

	std::ostream::fmtflags initialFlag;

	std::wofstream file;

};

#define LOGSUCCESS(...) Logger::get()->submitLogMessage(LogContext::createLogMessage(getTimeStamp(),getThreadId(),wrapClassName(typeid(*this).name()),LogType::LOG_SUCCESS,__VA_ARGS__))

#define LOGENGINE(...) Logger::get()->submitLogMessage(LogContext::createLogMessage(getTimeStamp(),getThreadId(),wrapClassName(typeid(*this).name()),LogType::LOG_ENGINE,__VA_ARGS__))

#define LOGUSER(...) Logger::get()->submitLogMessage(LogContext::createLogMessage(getTimeStamp(),getThreadId(),wrapClassName(typeid(*this).name()),LogType::LOG_USER,__VA_ARGS__))

#define LOGERROR(...) Logger::get()->submitLogMessage(LogContext::createLogMessage(getTimeStamp(),getThreadId(),L"",LogType::LOG_ERROR,__FILE__,L"function",__FUNCTION__,L"line",__LINE__,__VA_ARGS__)); \
throw std::runtime_error("check log.txt or console output for detailed information") \

#endif // !_LOGGER_H_