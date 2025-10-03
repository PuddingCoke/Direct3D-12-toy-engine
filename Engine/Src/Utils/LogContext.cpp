#include<Gear/Utils/LogContext.h>

#include<thread>

std::wstring getTimeStamp()
{
	const time_t currentTime = time(nullptr);

	tm localTime = {};

	localtime_s(&localTime, &currentTime);

	return L"[" + std::to_wstring(localTime.tm_hour) + L":" + std::to_wstring(localTime.tm_min) + L":" + std::to_wstring(localTime.tm_sec) + L"]";
}

std::wstring getThreadId()
{
	const auto id = std::this_thread::get_id();

	return L"{T" + std::to_wstring(*((unsigned int*)&id)) + L"}";
}

std::wstring wrapClassName(const char* const className)
{
	const size_t len = strnlen_s(className, UINT64_MAX);

	return L"(" + std::wstring(className, className + len) + L")";
}

LogContext::LogContext() :
	textColor{ L"" },
	displayColor{ L"" }
{
}

LogContext::~LogContext()
{
}

void LogContext::packRestArgument(void)
{
}

void LogContext::packArgument(const int32_t& arg)
{
	packInteger(arg);
}

void LogContext::packArgument(const int64_t& arg)
{
	packInteger(arg);
}

void LogContext::packArgument(const uint32_t& arg)
{
	packInteger(arg);
}

void LogContext::packArgument(const uint64_t& arg)
{
	packInteger(arg);
}

void LogContext::packArgument(const float_t& arg)
{
	packFloatPoint(arg);
}

void LogContext::packArgument(const double_t& arg)
{
	packFloatPoint(arg);
}

void LogContext::packArgument(const std::wstring& arg)
{
	setDisplayColor(textColor);

	consoleOutputStream << arg << L" ";

	fileOutputStream << arg << L" ";
}

void LogContext::packArgument(std::ios_base& __cdecl arg(std::ios_base&))
{
	consoleOutputStream << arg;

	fileOutputStream << arg;
}

void LogContext::packArgument(const LogColor& arg)
{
	if (textColor != arg)
	{
		textColor = arg;
	}
}

void LogContext::setDisplayColor(const LogColor& color)
{
	if (displayColor != color)
	{
		displayColor = color;

		consoleOutputStream << displayColor.code;
	}
}