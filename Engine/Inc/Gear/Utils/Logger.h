#pragma once

#ifndef _LOGGER_H_
#define _LOGGER_H_

#include<iostream>

#include<fstream>

#include<string>

#include<sstream>

#include"LogColor.h"

//output [hour:miniute:second]
std::wstring getTimeStamp();

//output {TXXXX}
std::wstring getThreadId();

//output (class name)
std::wstring wrapClassName(const char* const className);

enum class LogType
{
	LOG_SUCCESS,
	LOG_ERROR,
	LOG_ENGINE,
	LOG_USER
};

struct LogHeader
{
	const std::wstring timeStamp;

	const std::wstring threadId;

	const std::wstring className;

	const LogType type;
};

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

	template<typename... Args>
	void logNormal(const LogHeader& header, const Args&... args);

	template<typename... Args>
	void logError(const LogHeader& header, const Args&... args);

private:

	friend class Gear;

	Logger();

	~Logger();

	template<typename Arg>
	static constexpr bool isInteger();

	template<typename Arg>
	static constexpr bool isFloatPoint();

	template<typename Arg>
	static constexpr bool isString();

	template<typename... Args>
	void log(const LogHeader& header, const Args&... args);

	template<typename First, typename... Rest>
	void printRestArgument(const First& first, const Rest&... rest);

	void printRestArgument();

	template<typename Arg>
	void printInteger(const Arg& arg);

	template<typename Arg>
	void printFloatPoint(const Arg& arg);

	template<typename Arg>
	void printArgument(const Arg& arg);

	void printArgument(const int32_t& arg);

	void printArgument(const int64_t& arg);

	void printArgument(const uint32_t& arg);

	void printArgument(const uint64_t& arg);

	void printArgument(const float_t& arg);

	void printArgument(const double_t& arg);

	void printArgument(const std::wstring& arg);

	void printArgument(std::ios_base& __cdecl arg(std::ios_base&));

	void printArgument(const LogColor& arg);

	void setDisplayColor(const LogColor& color);

	static Logger* instance;

	LogColor textColor;

	LogColor displayColor;

	std::wostringstream consoleOutputStream;

	std::wostringstream fileOutputStream;

	std::wofstream file;

};

#define LOGSUCCESS(...) Logger::get()->logNormal(LogHeader{getTimeStamp(),getThreadId(),wrapClassName(typeid(*this).name()),LogType::LOG_SUCCESS},__VA_ARGS__)

#define LOGENGINE(...) Logger::get()->logNormal(LogHeader{getTimeStamp(),getThreadId(),wrapClassName(typeid(*this).name()),LogType::LOG_ENGINE},__VA_ARGS__)

#define LOGUSER(...) Logger::get()->logNormal(LogHeader{getTimeStamp(),getThreadId(),wrapClassName(typeid(*this).name()),LogType::LOG_USER},__VA_ARGS__)

#define LOGERROR(...) \
Logger::get()->logError(LogHeader{getTimeStamp(),getThreadId(),L"",LogType::LOG_ERROR},__FILE__,L"function",__FUNCTION__,L"line",__LINE__,__VA_ARGS__); \
throw std::runtime_error("check log.txt or console output for detailed information") \

#endif // !_LOGGER_H_

template<typename ...Args>
inline void Logger::logNormal(const LogHeader& header, const Args & ...args)
{
	log(header, args...);
}

template<typename ...Args>
inline void Logger::logError(const LogHeader& header, const Args & ...args)
{
	logNormal(header, args ...);

	if (file.is_open())
	{
		file.close();
	}
}

template<typename Arg>
inline constexpr bool Logger::isInteger()
{
	return std::is_same<int32_t, Arg>::value ||
		std::is_same<int64_t, Arg>::value ||
		std::is_same<uint32_t, Arg>::value ||
		std::is_same<uint64_t, Arg>::value;
}

template<typename Arg>
inline constexpr bool Logger::isFloatPoint()
{
	return std::is_same<float_t, Arg>::value ||
		std::is_same<double_t, Arg>::value;
}

template<typename Arg>
inline constexpr bool Logger::isString()
{
	return std::is_same<std::string, Arg>::value;
}

template<typename ...Args>
inline void Logger::log(const LogHeader& header, const Args & ...args)
{
	textColor = { L"" };

	displayColor = { L"" };

	printArgument(LogColor::timeStampColor);

	printArgument(header.timeStamp);

	printArgument(LogColor::threadIdColor);

	printArgument(header.threadId);

	if (header.type != LogType::LOG_ERROR)
	{
		printArgument(LogColor::classNameColor);

		printArgument(header.className);
	}

	switch (header.type)
	{
	case LogType::LOG_SUCCESS:
		printArgument(LogColor::successColor);

		printArgument(L"<SUCCESS>");

		break;
	case LogType::LOG_ERROR:
		printArgument(LogColor::errorColor);

		printArgument(L"<ERROR>");

		break;
	case LogType::LOG_ENGINE:
		printArgument(LogColor::engineColor);

		printArgument(L"<ENGINE>");

		break;
	case LogType::LOG_USER:
		printArgument(LogColor::userColor);

		printArgument(L"<USER>");

		break;
	}

	printArgument(LogColor::defaultColor);

	printRestArgument(args...);

	consoleOutputStream << L"\n";

	std::wcout << consoleOutputStream.str();

	consoleOutputStream.str(L"");

	consoleOutputStream.clear();

	if (file.is_open())
	{
		fileOutputStream << L"\n";

		file << fileOutputStream.str();

		fileOutputStream.str(L"");

		fileOutputStream.clear();
	}
}

template<typename First, typename ...Rest>
inline void Logger::printRestArgument(const First& first, const Rest& ...rest)
{
	static_assert(!isString<First>(), "error input type is std::string");

	printArgument(first);

	printRestArgument(rest...);
}

template<typename Arg>
inline void Logger::printInteger(const Arg& arg)
{
	static_assert(isInteger<Arg>(), "error input type is not integer");

	setDisplayColor(LogColor::numericColor);

	//if it is hex mode then start with 0x
	if (consoleOutputStream.flags() & std::ios::hex)
	{
		consoleOutputStream << L"0x" << arg << L" ";

		if (file.is_open())
		{
			fileOutputStream << L"0x" << arg << L" ";
		}
	}
	else
	{
		consoleOutputStream << arg << L" ";

		if (file.is_open())
		{
			fileOutputStream << arg << L" ";
		}
	}
}

template<typename Arg>
inline void Logger::printFloatPoint(const Arg& arg)
{
	static_assert(isFloatPoint<Arg>(), "error input type is not float point");

	setDisplayColor(LogColor::numericColor);

	consoleOutputStream << arg << L" ";

	if (file.is_open())
	{
		fileOutputStream << arg << L" ";
	}
}

template<typename Arg>
inline void Logger::printArgument(const Arg& arg)
{
	setDisplayColor(textColor);

	consoleOutputStream << arg << L" ";

	if (file.is_open())
	{
		fileOutputStream << arg << L" ";
	}
}