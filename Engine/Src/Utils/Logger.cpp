#include<Gear/Utils/Logger.h>

#include<iostream>

#include<Windows.h>

Logger* Logger::instance = nullptr;

void Logger::submitLogMessage(const LogMessage& msg)
{
	instance->submitMessage(msg);
}

Logger::Logger() :
	isRunning(true)
{
	//to support multiple language first set global locale to .UTF-8
	//and then set console output code page to CP_UTF8
	//steps above will make sure console and log.txt have correct content
	std::locale::global(std::locale(".UTF-8"));

	SetConsoleOutputCP(CP_UTF8);

	file = std::wofstream(L"log.txt", std::ios_base::out | std::ios_base::trunc);

	worker = std::thread(&Logger::workerLoop, this);
}

Logger::~Logger()
{
	shutdown();

	if (file.is_open())
	{
		file.flush();

		file.close();
	}
}

void Logger::shutdown()
{
	{
		std::lock_guard<std::mutex> lockGuard(queueLock);

		isRunning = false;
	}

	cv.notify_one();

	if (worker.joinable())
	{
		worker.join();
	}

	while (!messages.empty())
	{
		LogMessage message = std::move(messages.front());

		messages.pop();

		temp = message.slot.str;

		{
			std::lock_guard<std::mutex> inUseLock(message.inUseMutex);

			message.slot.inUse = false;
		}

		message.inUseCV.notify_one();

		if (file.is_open())
		{
			std::wcout << temp << L"\n";

			file << temp << L"\n";
		}
	}
}

void Logger::submitMessage(const LogMessage& msg)
{
	{
		std::lock_guard<std::mutex> lockGuard(queueLock);

		messages.push(msg);
	}

	cv.notify_one();
}

void Logger::workerLoop()
{
	while (true)
	{
		std::unique_lock<std::mutex> lock(queueLock);

		cv.wait(lock, [this]() {return !messages.empty() || !isRunning; });

		if (!isRunning)
		{
			break;
		}

		while (!messages.empty())
		{
			LogMessage message = std::move(messages.front());

			messages.pop();

			lock.unlock();

			temp = message.slot.str;

			{
				std::lock_guard<std::mutex> inUseLock(message.inUseMutex);

				message.slot.inUse = false;
			}

			message.inUseCV.notify_one();

			std::wcout << temp << L"\n";

			file << temp << L"\n";

			lock.lock();

			if (message.type == LogType::LOG_ERROR)
			{
				isRunning = false;

				file.flush();

				file.close();

				break;
			}
		}
	}
}