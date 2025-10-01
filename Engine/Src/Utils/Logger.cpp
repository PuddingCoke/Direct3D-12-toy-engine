#include<Gear/Utils/Logger.h>

#include<iomanip>

#include<thread>

#include<Windows.h>

Logger* Logger::instance = nullptr;

const std::wstring Logger::black = L"\033[30m";

const std::wstring Logger::red = L"\033[31m";

const std::wstring Logger::green = L"\033[32m";

const std::wstring Logger::yellow = L"\033[33m";

const std::wstring Logger::blue = L"\033[34m";

const std::wstring Logger::magenta = L"\033[35m";

const std::wstring Logger::cyan = L"\033[36m";

const std::wstring Logger::white = L"\033[37m";

const std::wstring Logger::brightBlack = L"\033[90m";

const std::wstring Logger::brightRed = L"\033[91m";

const std::wstring Logger::brightGreen = L"\033[92m";

const std::wstring Logger::brightYellow = L"\033[93m";

const std::wstring Logger::brightBlue = L"\033[94m";

const std::wstring Logger::brightMagenta = L"\033[95m";

const std::wstring Logger::brightCyan = L"\033[96m";

const std::wstring Logger::brightWhite = L"\033[97m";

const std::wstring Logger::defaultColor = Logger::brightWhite;

const std::wstring Logger::timeStampColor = Logger::brightBlack;

const std::wstring Logger::threadIdColor = Logger::cyan;

const std::wstring Logger::classNameColor = Logger::yellow;

const std::wstring Logger::successColor = Logger::brightGreen;

const std::wstring Logger::errorColor = Logger::brightRed;

const std::wstring Logger::engineColor = Logger::brightYellow;

const std::wstring Logger::userColor = Logger::brightWhite;

const std::wstring Logger::numericColor = Logger::brightCyan;

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

bool Logger::isEscapeCode(const std::wstring& str)
{
	return str.find(L"\033[") != std::wstring::npos;
}

void Logger::printRestArgument()
{
	consoleOutputStream << defaultColor;
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
	if (isEscapeCode(arg))
	{
		currentColor = arg;
	}
	else
	{
		consoleOutputStream << currentColor << arg << " ";

		if (file.is_open())
		{
			fileOutputStream << arg << " ";
		}
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
