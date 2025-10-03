#pragma once

#ifndef _LOGCONTEXT_H_
#define _LOGCONTEXT_H_

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

struct LogMessage
{
	const std::wstring consoleOutputStr;

	const std::wstring fileOutputStr;

	const LogType type;
};

class LogContext
{
public:

	enum class IntegerMode
	{
		DEC, HEX
	};

	LogContext(const LogContext&) = delete;

	void operator=(const LogContext&) = delete;

	LogContext();

	~LogContext();

	template<typename... Args>
	static LogMessage createLogMessage(const std::wstring& timeStamp, const std::wstring& threadId, const std::wstring& className, const LogType& type, const Args&... args);

private:

	template<typename... Args>
	LogMessage getLogMessage(const std::wstring& timeStamp, const std::wstring& threadId, const std::wstring& className, const LogType& type, const Args&... args);

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
	void packArgument(const Arg& arg);

	//literal wchar_t array
	template<size_t N>
	void packArgument(const wchar_t (&arg)[N]);
	
	//wstring
	void packArgument(const std::wstring& arg);

	//const wchar*
	void packArgument(const wchar_t* arg);

	//singed 32bit integer
	void packArgument(const int32_t& arg);

	//signed 64bit integer
	void packArgument(const int64_t& arg);

	//unsigned 32bit integer
	void packArgument(const uint32_t& arg);

	//unsigned 64bit integer
	void packArgument(const uint64_t& arg);

	//float
	void packArgument(const float_t& arg);

	//double
	void packArgument(const double_t& arg);

	//change integer mode
	void packArgument(const IntegerMode& mode);

	//change text color
	void packArgument(const LogColor& arg);

	//change display color
	void setDisplayColor(const LogColor& color);

	IntegerMode integerMode;

	LogColor textColor;

	LogColor displayColor;

	std::wstring consoleOutputStr;

	std::wstring fileOutputStr;
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

	return LogMessage{ consoleOutputStr,fileOutputStr,type };
}

template<typename First, typename ...Rest>
inline void LogContext::packRestArgument(const First& first, const Rest& ...rest)
{
	static_assert(!std::is_same<std::string, First>::value, "error input type is std::string");

	static_assert(!isNativeString<First>::value, "error input type is native string");

	packArgument(first);

	packRestArgument(rest...);
}

template<typename Arg>
inline void LogContext::packArgument(const Arg& arg)
{
	static_assert(0, "not supported type");

	//test use only
	/*setDisplayColor(textColor);

	const std::string ty = typeid(Arg).name();

	packArgument(std::wstring(ty.cbegin(), ty.cend()));*/
}

template<size_t N>
inline void LogContext::packArgument(const wchar_t (&arg)[N])
{
	setDisplayColor(textColor);

	consoleOutputStr += arg;

	consoleOutputStr += L" ";

	fileOutputStr += arg;

	fileOutputStr += L" ";
}

#endif // !_LOGCONTEXT_H_
