#include<Gear/Utils/Logger.h>

#include<Windows.h>

std::wstring wrapClassName(const char* const className)
{
	const size_t len = strnlen_s(className, UINT64_MAX);

	return L"(" + std::wstring(className, className + len) + L")";
}

Logger* Logger::instance = nullptr;

Logger* Logger::get()
{
	return instance;
}

void Logger::submitLogMessage(const LogMessage& msg)
{
	std::wcout << msg.messageStr << L"\n";

	if (file.is_open())
	{
		file << msg.messageStr << L"\n";
	}

	if (msg.type == LogType::LOG_ERROR && file.is_open())
	{
		file.close();
	}
}

//to support multiple language first set global locale to .UTF-8
//and then set console output code page to CP_UTF8
//steps above will make sure console and log.txt have correct content
Logger::Logger()
{
	std::locale::global(std::locale(".UTF-8"));

	SetConsoleOutputCP(CP_UTF8);

	file = std::wofstream(L"log.txt", std::ios_base::out | std::ios_base::trunc);
}

Logger::~Logger()
{
	if (file.is_open())
	{
		file.close();
	}
}
