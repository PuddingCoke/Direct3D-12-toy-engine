#pragma once

#ifndef _LOGGER_H_
#define _LOGGER_H_

#include<iostream>
#include<fstream>
#include<string>
#include<regex>
#include<iomanip>
#include<Windows.h>

/// <summary>
/// a simple logger that output text with colors depend on different situations
/// and numeric value like int32_t uint32_t float_t have special color
/// since i want to use utf-8 encoding so std::string is not supported
/// do not use logger in any loop or it will have huge impact on performance!
/// </summary>
class Logger
{
public:

	Logger(const Logger&) = delete;

	void operator=(const Logger&) = delete;

	static Logger* get();

	template<typename... Args>
	void logSuccess(const Args&... args);

	template<typename... Args>
	void logError(const Args&... args);

	template<typename... Args>
	void logEngine(const Args&... args);

	template<typename... Args>
	void logUser(const Args&... args);

	template<typename... Args>
	void log(const Args&... args);

	static const std::wstring reset;

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

	static const std::wstring& resetColor();

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

	static const std::wregex escapeCodeRegex;

	std::wstring targetColor;

	std::wstring originalColor;

	const std::wstring numericColor;

	const std::wstring successColor;

	const std::wstring errorColor;

	const std::wstring engineColor;

	const std::wstring userColor;

	std::wofstream file;

};

#define LOGSUCCESS(...) Logger::get()->logSuccess("[",typeid(*this).name(),"]",__VA_ARGS__)

#define LOGERROR(...) Logger::get()->logError("error occur at",__FILE__,"function",__FUNCTION__,"line",__LINE__,__VA_ARGS__); throw "see log.txt or console output for detailed information"

#define LOGENGINE(...) Logger::get()->logEngine("[",typeid(*this).name(),"]",__VA_ARGS__)

#define LOGUSER(...) Logger::get()->logUser(__VA_ARGS__)

#define LOG(...) Logger::get()->log(__VA_ARGS__)

#endif // !_LOGGER_H_

template<typename ...Args>
inline void Logger::logSuccess(const Args & ...args)
{
	targetColor = successColor;

	originalColor = targetColor;

	log(args...);
}

template<typename ...Args>
inline void Logger::logError(const Args & ...args)
{
	targetColor = errorColor;

	originalColor = targetColor;

	log(args...);

	if (file.is_open())
	{
		file.close();
	}
}

template<typename ...Args>
inline void Logger::logEngine(const Args & ...args)
{
	targetColor = engineColor;

	originalColor = targetColor;

	log(args...);
}

template<typename ...Args>
inline void Logger::logUser(const Args & ...args)
{
	targetColor = userColor;

	originalColor = targetColor;

	log(args...);
}

template<typename ...Args>
inline void Logger::log(const Args & ...args)
{
	printRestArgument(args...);

	std::wcout << "\n";

	if (file.is_open())
	{
		file << "\n";
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
	if (std::wcout.flags() & std::ios::hex)
	{
		std::wcout << numericColor << "0x" << arg << " ";

		if (file.is_open())
		{
			file << "0x" << arg << " ";
		}
	}
	else
	{
		std::wcout << numericColor << arg << " ";

		if (file.is_open())
		{
			file << arg << " ";
		}
	}
}

template<typename Arg>
inline void Logger::printFloatPoint(const Arg& arg)
{
	static_assert(isFloatPoint<Arg>(), "error input type is not float point");

	std::wcout << numericColor << arg << " ";

	if (file.is_open())
	{
		file << arg << " ";
	}
}

template<typename First, typename ...Rest>
inline void Logger::printRestArgument(const First& first, const Rest & ...rest)
{
	static_assert(!isString<First>(), "error input type is std::string");

	printArgument(first);

	printRestArgument(rest...);
}

template<typename Arg>
inline void Logger::printArgument(const Arg& arg)
{
	std::wcout << targetColor << arg << " ";

	if (file.is_open())
	{
		file << arg << " ";
	}
}


