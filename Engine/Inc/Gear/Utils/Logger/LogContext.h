#pragma once

#ifndef _LOGCONTEXT_H_
#define _LOGCONTEXT_H_

#include"LogColor.h"

#include<thread>

enum class LogType
{
	LOG_SUCCESS,
	LOG_ERROR,
	LOG_ENGINE,
	LOG_USER
};

struct LogMessage
{
	const std::wstring str;

	const LogType type;
};

class LogContext
{
public:

	enum class IntegerMode
	{
		DEC, HEX
	};

	struct FloatPrecision
	{
		FloatPrecision(const int precision = 5);

		int precision;
	};

	LogContext(const LogContext&) = delete;

	void operator=(const LogContext&) = delete;

	LogContext();

	~LogContext();

	template<typename... Args>
	static LogMessage createLogMessage(const std::wstring& className, const LogType& type, const Args&... args);

private:

	template<typename... Args>
	LogMessage getLogMessage(const std::wstring& className, const LogType& type, const Args&... args);

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

	//forbidden default case
	template<typename Arg>
	void packArgument(const Arg& arg);

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

	template<typename Arg>
	void packFloatPoint(const Arg& arg);

	//float
	void packArgument(const float_t& arg);

	//double
	void packArgument(const double_t& arg);

	//change integer mode
	void packArgument(const IntegerMode& mode);

	//change float precision
	void packArgument(const FloatPrecision& precision);

	//change text color
	void packArgument(const LogColor& arg);

	//change display color
	void setDisplayColor(const LogColor& color);

	void resetState();

	IntegerMode integerMode;

	FloatPrecision floatPrecision;

	LogColor textColor;

	LogColor displayColor;

	std::wstring messageStr;
};

template<typename ...Args>
inline LogMessage LogContext::createLogMessage(const std::wstring& className, const LogType& type, const Args & ...args)
{
	thread_local LogContext context;

	context.resetState();

	return context.getLogMessage(className, type, args...);
}

template<typename ...Args>
inline LogMessage LogContext::getLogMessage(const std::wstring& className, const LogType& type, const Args & ...args)
{
	{
		const time_t currentTime = time(nullptr);

		tm localTime = {};

		localtime_s(&localTime, &currentTime);

		/*5[8] 5{T10} */
		//conservative length = 5+2+8+1+5+2+1+10+1+1 = 36
		const size_t headerStrLen = 36ull;

		wchar_t headerStr[headerStrLen] = {};

		const std::thread::id id = std::this_thread::get_id();

		const uint32_t threadId = *(uint32_t*)&id;

		swprintf_s(headerStr, headerStrLen, L"%s[%d:%d:%d] %s{T%u} ", LogColor::timeStampColor.code, localTime.tm_hour, localTime.tm_min, localTime.tm_sec,
			LogColor::threadIdColor.code, threadId);

		messageStr += headerStr;
	}

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

	return LogMessage{ std::move(messageStr),type };
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

template<typename Arg>
inline void LogContext::packFloatPoint(const Arg& arg)
{
	setDisplayColor(LogColor::numericColor);

	wchar_t buff[_CVTBUFSIZE] = {};

	swprintf_s(buff, _CVTBUFSIZE, L"%.*f ", floatPrecision.precision, arg);

	messageStr += buff;
}

#endif // !_LOGCONTEXT_H_
