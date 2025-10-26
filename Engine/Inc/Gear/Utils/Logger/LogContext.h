﻿#pragma once

#ifndef _UTILS_LOGGER_LOGCONTEXT_H_
#define _UTILS_LOGGER_LOGCONTEXT_H_

#include"LogColor.h"

#include<thread>

#include<condition_variable>

namespace Utils
{
	namespace Logger
	{
		enum class LogType
		{
			LOG_SUCCESS,
			LOG_ERROR,
			LOG_ENGINE,
			LOG_USER
		};

		struct BufferSlot
		{
			BufferSlot();

			std::wstring str;

			bool inUse;
		};

		struct LogMessage
		{
			BufferSlot& slot;

			std::mutex& inUseMutex;

			std::condition_variable& inUseCV;

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
				FloatPrecision(const int32_t precision = 5);

				int32_t precision;
			};

			LogContext(const LogContext&) = delete;

			void operator=(const LogContext&) = delete;

			LogContext();

			~LogContext();

			template<typename... Args>
			static LogMessage createLogMessage(const wchar_t* const functionName, const LogType& type, const Args&... args);

		private:

			template<typename... Args>
			LogMessage getLogMessage(const wchar_t* const functionName, const LogType& type, const Args&... args);

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

			static constexpr size_t slotNum = 128;

			BufferSlot* const slots;

			uint32_t writeIndex;

			std::wstring* messageStr;

			std::mutex inUseMutex;

			std::condition_variable inUseCV;

		};
	}
}

template<typename ...Args>
inline Utils::Logger::LogMessage Utils::Logger::LogContext::createLogMessage(const wchar_t* const functionName, const LogType& type, const Args & ...args)
{
	thread_local LogContext context;

	context.resetState();

	return context.getLogMessage(functionName, type, args...);
}

template<typename ...Args>
inline Utils::Logger::LogMessage Utils::Logger::LogContext::getLogMessage(const wchar_t* const functionName, const LogType& type, const Args & ...args)
{
	if (slots[writeIndex].inUse)
	{
		std::unique_lock<std::mutex> inUseLock(inUseMutex);

		inUseCV.wait(inUseLock, [this]() {return !slots[writeIndex].inUse; });
	}

	messageStr = &slots[writeIndex].str;

	slots[writeIndex].inUse = true;

	const LogMessage message = { slots[writeIndex],inUseMutex,inUseCV,type };

	writeIndex = (writeIndex + 1u) % slotNum;

	messageStr->clear();

	{
		const time_t currentTime = time(nullptr);

		tm localTime = {};

		localtime_s(&localTime, &currentTime);

		//headerStrLen = 5+2+8+1+5+2+1+10+1+5+2+length(functionName)+1+1
		//			   = 44+length(functionName)
		const size_t headerStrLen = 256ull;

		wchar_t headerStr[headerStrLen] = {};

		const std::thread::id id = std::this_thread::get_id();

		const uint32_t threadId = *(uint32_t*)&id;

		swprintf_s(headerStr, headerStrLen, L"%s[%d:%d:%d] %s{T%u} %s(%s) ", LogColor::timeStampColor.code, localTime.tm_hour, localTime.tm_min, localTime.tm_sec,
			LogColor::threadIdColor.code, threadId, LogColor::functionNameColor.code, functionName);

		*messageStr += headerStr;
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

	return message;
}

template<typename First, typename ...Rest>
inline void Utils::Logger::LogContext::packRestArgument(const First& first, const Rest& ...rest)
{
	static_assert(!std::is_same<std::string, First>::value, "error input type is std::string");

	static_assert(!isNativeString<First>::value, "error input type is native string");

	packArgument(first);

	packRestArgument(rest...);
}

template<typename Arg>
inline void Utils::Logger::LogContext::packArgument(const Arg& arg)
{
	static_assert(0, "not supported type");

	//test use only
	/*setDisplayColor(textColor);

	const std::string ty = typeid(Arg).name();

	packArgument(std::wstring(ty.cbegin(), ty.cend()));*/
}

template<typename Arg>
inline void Utils::Logger::LogContext::packFloatPoint(const Arg& arg)
{
	setDisplayColor(LogColor::numericColor);

	wchar_t buff[_CVTBUFSIZE] = {};

	swprintf_s(buff, _CVTBUFSIZE, L"%.*f ", floatPrecision.precision, arg);

	*messageStr += buff;
}

#endif // !_UTILS_LOGGER_LOGCONTEXT_H_
