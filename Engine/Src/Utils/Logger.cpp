#include<Gear/Utils/Logger.h>

#include<iomanip>

#include<thread>

#include<Windows.h>

std::wstring getTimeStamp()
{
	const time_t currentTime = time(nullptr);

	tm localTime = {};

	localtime_s(&localTime, &currentTime);

	std::wostringstream timeStampStream;

	timeStampStream << L"[" << localTime.tm_hour << L":" << localTime.tm_min << L":" << localTime.tm_sec << L"]";

	return timeStampStream.str();
}

std::wstring getThreadId()
{
	std::wostringstream threadIdStream;

	threadIdStream << L"{T" << std::this_thread::get_id() << L"}";

	return threadIdStream.str();
}

std::wstring wrapClassName(const char* const className)
{
	std::wostringstream classNameStream;

	classNameStream << L"(" << className << L")";

	return classNameStream.str();
}

Logger* Logger::instance = nullptr;

Logger* Logger::get()
{
	return instance;
}

Logger::Logger()
{
	//to support multiple language first set locale to .UTF-8
	//and then set console output code page to CP_UTF8
	std::locale::global(std::locale(".UTF-8"));

	SetConsoleOutputCP(CP_UTF8);

	file.open(L"log.txt", std::ios_base::out | std::ios_base::trunc);
}

Logger::~Logger()
{
	if (file.is_open())
	{
		file.close();
	}
}

void Logger::printRestArgument()
{
	printArgument(LogColor::defaultColor);
}

void Logger::printArgument(const int32_t& arg)
{
	printInteger(arg);
}

void Logger::printArgument(const int64_t& arg)
{
	printInteger(arg);
}

void Logger::printArgument(const uint32_t& arg)
{
	printInteger(arg);
}

void Logger::printArgument(const uint64_t& arg)
{
	printInteger(arg);
}

void Logger::printArgument(const float_t& arg)
{
	printFloatPoint(arg);
}

void Logger::printArgument(const double_t& arg)
{
	printFloatPoint(arg);
}

void Logger::printArgument(const std::wstring& arg)
{
	setDisplayColor(textColor);

	consoleOutputStream << arg << L" ";

	if (file.is_open())
	{
		fileOutputStream << arg << L" ";
	}
}

void Logger::printArgument(std::ios_base& __cdecl arg(std::ios_base&))
{
	consoleOutputStream << arg;

	if (file.is_open())
	{
		fileOutputStream << arg;
	}
}

void Logger::printArgument(const LogColor& arg)
{
	if (textColor != arg)
	{
		textColor = arg;
	}
}

void Logger::setDisplayColor(const LogColor& color)
{
	if (displayColor != color)
	{
		displayColor = color;

		consoleOutputStream << displayColor.code;
	}
}
