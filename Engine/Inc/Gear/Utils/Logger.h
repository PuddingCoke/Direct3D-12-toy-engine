#pragma once

#ifndef _LOGGER_H_
#define _LOGGER_H_

#include<iostream>

#include<fstream>

#include<string>

#include<sstream>

//output [hour:miniute:second]
std::wstring getTimeStamp();

//output {TXXXX}
std::wstring getThreadId();

//output (class name)
std::wstring wrapClassName(const char* const className);

/// <summary>
/// a simple logger that output text with colors depend on different situations
/// and numeric value like int32_t uint32_t float_t have special color
/// since i want to use utf-8 encoding so std::string is not supported
/// do not use logger in loop or there will be performance impair!
/// </summary>
class Logger
{
public:

	Logger(const Logger&) = delete;

	void operator=(const Logger&) = delete;

	static Logger* get();

	template<typename... Args>
	void logNormal(const Args&... args);

	template<typename... Args>
	void logError(const Args&... args);

	static const std::wstring black;

	static const std::wstring red;

	static const std::wstring green;

	static const std::wstring yellow;

	static const std::wstring blue;

	static const std::wstring magenta;

	static const std::wstring cyan;

	static const std::wstring white;

	static const std::wstring brightBlack;

	static const std::wstring brightRed;

	static const std::wstring brightGreen;

	static const std::wstring brightYellow;

	static const std::wstring brightBlue;

	static const std::wstring brightMagenta;

	static const std::wstring brightCyan;

	static const std::wstring brightWhite;

	//main text
	static const std::wstring defaultColor;

	static const std::wstring timeStampColor;

	static const std::wstring threadIdColor;

	static const std::wstring classNameColor;

	static const std::wstring successColor;

	static const std::wstring errorColor;

	static const std::wstring engineColor;

	static const std::wstring userColor;

	//numeric value
	static const std::wstring numericColor;

private:

	friend class Gear;

	Logger();

	~Logger();

	static bool isEscapeCode(const std::wstring& str);

	template<typename Arg>
	static constexpr bool isInteger();

	template<typename Arg>
	static constexpr bool isFloatPoint();

	template<typename Arg>
	static constexpr bool isString();

	template<typename... Args>
	void log(const Args&... args);

	template<typename Arg>
	void printInteger(const Arg& arg);

	template<typename Arg>
	void printFloatPoint(const Arg& arg);

	void printRestArgument();

	template<typename First, typename... Rest>
	void printRestArgument(const First& first, const Rest&... rest);

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

	static Logger* instance;

	std::wstring currentColor;

	std::wostringstream consoleOutputStream;

	std::wostringstream fileOutputStream;

	std::wofstream file;

};

#define LOGSUCCESS(...) Logger::get()->logNormal(Logger::timeStampColor,getTimeStamp(),Logger::threadIdColor,getThreadId(),Logger::classNameColor,wrapClassName(typeid(*this).name()),Logger::successColor,"<SUCCESS>",Logger::defaultColor,__VA_ARGS__)

#define LOGENGINE(...) Logger::get()->logNormal(Logger::timeStampColor,getTimeStamp(),Logger::threadIdColor,getThreadId(),Logger::classNameColor,wrapClassName(typeid(*this).name()),Logger::engineColor,"<ENGINE>",Logger::defaultColor,__VA_ARGS__)

#define LOGUSER(...) Logger::get()->logNormal(Logger::timeStampColor,getTimeStamp(),Logger::threadIdColor,getThreadId(),Logger::classNameColor,wrapClassName(typeid(*this).name()),Logger::userColor,"<USER>",Logger::defaultColor,__VA_ARGS__)

#define LOGERROR(...) Logger::get()->logError(Logger::timeStampColor,getTimeStamp(),Logger::threadIdColor,getThreadId(),Logger::errorColor,"<ERROR>",Logger::defaultColor,__FILE__,"function",__FUNCTION__,"line",__LINE__,__VA_ARGS__)

#endif // !_LOGGER_H_

template<typename ...Args>
inline void Logger::logNormal(const Args & ...args)
{
	log(args...);
}

template<typename ...Args>
inline void Logger::logError(const Args & ...args)
{
	logNormal(args ...);

	if (file.is_open())
	{
		file.close();
	}

	throw std::runtime_error("check log.txt or console output for detailed information");
}

template<typename ...Args>
inline void Logger::log(const Args & ...args)
{
	currentColor = defaultColor;

	printRestArgument(args...);

	consoleOutputStream << "\n";

	std::wcout << consoleOutputStream.str();

	consoleOutputStream.str(L"");

	consoleOutputStream.clear();

	if (file.is_open())
	{
		fileOutputStream << "\n";

		file << fileOutputStream.str();

		fileOutputStream.str(L"");

		fileOutputStream.clear();
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

template<typename Arg>
inline void Logger::printInteger(const Arg& arg)
{
	static_assert(isInteger<Arg>(), "error input type is not integer");

	//if it is hex mode then start with 0x
	if (consoleOutputStream.flags() & std::ios::hex)
	{
		consoleOutputStream << numericColor << "0x" << arg << " ";

		if (file.is_open())
		{
			fileOutputStream << "0x" << arg << " ";
		}
	}
	else
	{
		consoleOutputStream << numericColor << arg << " ";

		if (file.is_open())
		{
			fileOutputStream << arg << " ";
		}
	}
}

template<typename Arg>
inline void Logger::printFloatPoint(const Arg& arg)
{
	static_assert(isFloatPoint<Arg>(), "error input type is not float point");

	consoleOutputStream << numericColor << arg << " ";

	if (file.is_open())
	{
		fileOutputStream << arg << " ";
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
inline void Logger::printArgument(const Arg& arg)
{
	consoleOutputStream << currentColor << arg << " ";

	if (file.is_open())
	{
		fileOutputStream << arg << " ";
	}
}


