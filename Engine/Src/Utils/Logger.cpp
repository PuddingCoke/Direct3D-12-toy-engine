#include<Gear/Utils/Logger.h>

#include<Gear/Utils/Internal/LoggerInternal.h>

#include<iostream>

#include<fstream>

#include<queue>

#define NOMINMAX

#include<Windows.h>

namespace
{
	class LoggerPrivate
	{
	public:

		LoggerPrivate(const LoggerPrivate&) = delete;

		void operator=(const LoggerPrivate&) = delete;

		LoggerPrivate() :
			isRunning(true)
		{
			//为了支持多语言
			//首先设置locale为.UTF-8
			//接着设置控制台输出代码页为CP_UTF8
			std::locale::global(std::locale(".UTF-8"));

			SetConsoleOutputCP(CP_UTF8);

			file = std::wofstream(L"log.txt", std::ios_base::out | std::ios_base::trunc);

			worker = std::thread(&LoggerPrivate::workerLoop, this);
		}

		~LoggerPrivate()
		{
			shutdown();

			if (file.is_open())
			{
				file.flush();

				file.close();
			}
		}

		void submitLogMessage(const Utils::Logger::LogMessage& msg)
		{
			{
				std::lock_guard<std::mutex> lockGuard(queueLock);

				messages.push(msg);
			}

			cv.notify_one();
		}

	private:

		std::wofstream file;

		std::queue<Utils::Logger::LogMessage> messages;

		bool isRunning;

		std::mutex queueLock;

		std::condition_variable cv;

		std::wstring temp;

		std::thread worker;

		void shutdown()
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
				Utils::Logger::LogMessage message = std::move(messages.front());

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

		void workerLoop()
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
					Utils::Logger::LogMessage message = std::move(messages.front());

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

					if (message.type == Utils::Logger::LogType::LOG_ERROR)
					{
						isRunning = false;

						file.flush();

						file.close();

						break;
					}
				}
			}
		}

	}*pvt = nullptr;

}

void Utils::Logger::submitLogMessage(const LogMessage& msg)
{
	pvt->submitLogMessage(msg);
}

void Utils::Logger::Internal::initialize()
{
	pvt = new LoggerPrivate();
}

void Utils::Logger::Internal::release()
{
	if (pvt)
	{
		delete pvt;
	}
}