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

	return L"{T" + std::to_wstring(*((uint32_t*)&id)) + L"}";
}

std::wstring wrapClassName(const char* const className)
{
	const size_t len = strnlen_s(className, UINT64_MAX);

	return L"(" + std::wstring(className, className + len) + L")";
}

LogContext::LogContext() :
	textColor{ L"" },
	displayColor{ L"" },
	integerMode(IntegerMode::DEC)
{
}

LogContext::~LogContext()
{
}

void LogContext::packRestArgument()
{
}

void LogContext::packArgument(const std::wstring& arg)
{
	setDisplayColor(textColor);

	consoleOutputStr += arg + L" ";

	fileOutputStr += arg + L" ";
}

void LogContext::packArgument(const wchar_t* arg)
{
	setDisplayColor(textColor);

	consoleOutputStr += arg;

	consoleOutputStr += L" ";

	fileOutputStr += arg;

	fileOutputStr += L" ";
}

void LogContext::packArgument(const int32_t& arg)
{
	setDisplayColor(LogColor::numericColor);

	if (integerMode == IntegerMode::HEX)
	{
		wchar_t buff[9] = {};

		_itow_s(arg, buff, 9, 16);

		consoleOutputStr += L"0x";

		consoleOutputStr += buff;

		fileOutputStr += L"0x";

		fileOutputStr += buff;
	}
	else
	{
		consoleOutputStr += std::to_wstring(arg);

		fileOutputStr += std::to_wstring(arg);
	}

	consoleOutputStr += L" ";

	fileOutputStr += L" ";
}

void LogContext::packArgument(const int64_t& arg)
{
	setDisplayColor(LogColor::numericColor);

	if (integerMode == IntegerMode::HEX)
	{
		wchar_t buff[17] = {};

		_i64tow_s(arg, buff, 17, 16);

		consoleOutputStr += L"0x";

		consoleOutputStr += buff;

		fileOutputStr += L"0x";

		fileOutputStr += buff;
	}
	else
	{
		consoleOutputStr += std::to_wstring(arg);

		fileOutputStr += std::to_wstring(arg);
	}

	consoleOutputStr += L" ";

	fileOutputStr += L" ";
}

void LogContext::packArgument(const uint32_t& arg)
{
	setDisplayColor(LogColor::numericColor);

	if (integerMode == IntegerMode::HEX)
	{
		wchar_t buff[9] = {};

		_ultow_s(arg, buff, 9, 16);

		consoleOutputStr += L"0x";

		consoleOutputStr += buff;

		fileOutputStr += L"0x";

		fileOutputStr += buff;
	}
	else
	{
		consoleOutputStr += std::to_wstring(arg);

		fileOutputStr += std::to_wstring(arg);
	}

	consoleOutputStr += L" ";

	fileOutputStr += L" ";
}

void LogContext::packArgument(const uint64_t& arg)
{
	setDisplayColor(LogColor::numericColor);

	if (integerMode == IntegerMode::HEX)
	{
		wchar_t buff[17] = {};

		_ui64tow_s(arg, buff, 17, 16);

		consoleOutputStr += L"0x";

		consoleOutputStr += buff;

		fileOutputStr += L"0x";

		fileOutputStr += buff;
	}
	else
	{
		consoleOutputStr += std::to_wstring(arg);

		fileOutputStr += std::to_wstring(arg);
	}

	consoleOutputStr += L" ";

	fileOutputStr += L" ";
}

void LogContext::packArgument(const float_t& arg)
{
	setDisplayColor(LogColor::numericColor);

	consoleOutputStr += std::to_wstring(arg) + L" ";

	fileOutputStr += std::to_wstring(arg) + L" ";
}

void LogContext::packArgument(const double_t& arg)
{
	setDisplayColor(LogColor::numericColor);

	consoleOutputStr += std::to_wstring(arg) + L" ";

	fileOutputStr += std::to_wstring(arg) + L" ";
}

void LogContext::packArgument(const IntegerMode& mode)
{
	integerMode = mode;
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

		consoleOutputStr += displayColor.code;
	}
}