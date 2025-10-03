#pragma once

#ifndef _LOGCONTEXT_H_
#define _LOGCONTEXT_H_

#include"LogColor.h"

#include<sstream>

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

struct LogMessage
{
	const std::wstring consoleOutputStr;

	const std::wstring fileOutputStr;

	const LogType type;
};

class LogContext
{
public:

	LogContext(const LogContext&) = delete;

	void operator=(const LogContext&) = delete;

	LogContext();

	~LogContext();

	template<typename... Args>
	static LogMessage createLogMessage(const std::wstring& timeStamp, const std::wstring& threadId, const std::wstring& className, const LogType& type, const Args&... args);

private:

	template<typename... Args>
	LogMessage getLogMessage(const std::wstring& timeStamp, const std::wstring& threadId, const std::wstring& className, const LogType& type, const Args&... args);

	template<typename Arg>
	static constexpr bool isInteger();

	template<typename Arg>
	static constexpr bool isFloatPoint();

	template<typename Arg>
	static constexpr bool isString();

	template<typename T>
	struct isNativeString :std::false_type {};

	template<size_t N>
	struct isNativeString<const char[N]> : std::true_type {};

	template<size_t N>
	struct isNativeString<char[N]> : std::true_type {};

	template<>
	struct isNativeString<const char*> : std::true_type {};

	template<>
	struct isNativeString<char*> : std::true_type {};

	template<typename First, typename... Rest>
	void packRestArgument(const First& first, const Rest&... rest);

	void packRestArgument();

	template<typename Arg>
	void packInteger(const Arg& arg);

	template<typename Arg>
	void packFloatPoint(const Arg& arg);

	template<typename Arg>
	void packArgument(const Arg& arg);

	void packArgument(const int32_t& arg);

	void packArgument(const int64_t& arg);

	void packArgument(const uint32_t& arg);

	void packArgument(const uint64_t& arg);

	void packArgument(const float_t& arg);

	void packArgument(const double_t& arg);

	void packArgument(const std::wstring& arg);

	void packArgument(std::ios_base& __cdecl arg(std::ios_base&));

	void packArgument(const LogColor& arg);

	void setDisplayColor(const LogColor& color);

	LogColor textColor;

	LogColor displayColor;

	std::wostringstream consoleOutputStream;

	std::wostringstream fileOutputStream;
};

template<typename ...Args>
inline LogMessage LogContext::createLogMessage(const std::wstring& timeStamp, const std::wstring& threadId, const std::wstring& className, const LogType& type, const Args & ...args)
{
	return LogContext().getLogMessage(timeStamp, threadId, className, type, args...);
}

template<typename ...Args>
inline LogMessage LogContext::getLogMessage(const std::wstring& timeStamp, const std::wstring& threadId, const std::wstring& className, const LogType& type, const Args & ...args)
{
	packArgument(LogColor::timeStampColor);

	packArgument(timeStamp);

	packArgument(LogColor::threadIdColor);

	packArgument(threadId);

	if (type != LogType::LOG_ERROR)
	{
		packArgument(LogColor::classNameColor);

		packArgument(className);
	}

	switch (type)
	{
	case LogType::LOG_SUCCESS:
		packArgument(LogColor::successColor);

		packArgument(L"<SUCCESS>");

		break;
	case LogType::LOG_ERROR:
		packArgument(LogColor::errorColor);

		packArgument(L"<ERROR>");

		break;
	case LogType::LOG_ENGINE:
		packArgument(LogColor::engineColor);

		packArgument(L"<ENGINE>");

		break;
	case LogType::LOG_USER:
		packArgument(LogColor::userColor);

		packArgument(L"<USER>");

		break;
	}

	packArgument(LogColor::defaultColor);

	packRestArgument(args...);

	return LogMessage{ consoleOutputStream.str(),fileOutputStream.str(),type };
}

template<typename Arg>
inline constexpr bool LogContext::isInteger()
{
	return std::is_same<int32_t, Arg>::value ||
		std::is_same<int64_t, Arg>::value ||
		std::is_same<uint32_t, Arg>::value ||
		std::is_same<uint64_t, Arg>::value;
}

template<typename Arg>
inline constexpr bool LogContext::isFloatPoint()
{
	return std::is_same<float_t, Arg>::value ||
		std::is_same<double_t, Arg>::value;
}

template<typename Arg>
inline constexpr bool LogContext::isString()
{
	return std::is_same<std::string, Arg>::value;
}

template<typename First, typename ...Rest>
inline void LogContext::packRestArgument(const First& first, const Rest& ...rest)
{
	static_assert(!isString<First>(), "error input type is std::string");

	static_assert(!isNativeString<First>::value, "error input type is native string");

	packArgument(first);

	packRestArgument(rest...);
}

template<typename Arg>
inline void LogContext::packInteger(const Arg& arg)
{
	static_assert(isInteger<Arg>(), "error input type is not integer");

	setDisplayColor(LogColor::numericColor);

	//if it is hex mode then start with 0x
	if (consoleOutputStream.flags() & std::ios::hex)
	{
		consoleOutputStream << L"0x" << arg << L" ";

		fileOutputStream << L"0x" << arg << L" ";

	}
	else
	{
		consoleOutputStream << arg << L" ";

		fileOutputStream << arg << L" ";
	}
}

template<typename Arg>
inline void LogContext::packFloatPoint(const Arg& arg)
{
	static_assert(isFloatPoint<Arg>(), "error input type is not float point");

	setDisplayColor(LogColor::numericColor);

	consoleOutputStream << arg << L" ";

	fileOutputStream << arg << L" ";
}

template<typename Arg>
inline void LogContext::packArgument(const Arg& arg)
{
	setDisplayColor(textColor);

	consoleOutputStream << arg << L" ";

	fileOutputStream << arg << L" ";
}

#endif // !_LOGCONTEXT_H_
