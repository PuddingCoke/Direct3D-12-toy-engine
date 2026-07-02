#include<Gear/Utils/Logger.h>

#include<Gear/Utils/Internal/LoggerInternal.h>

#include<fileapi.h>

#include<consoleapi.h>

#include<locale>

#include<string>

#include<queue>

namespace Gear::Utils::Logger
{
	namespace Internal
	{
		class LoggerImpl
		{
		public:

			LoggerImpl(const LoggerImpl&) = delete;

			void operator=(const LoggerImpl&) = delete;

			LoggerImpl() :
				isRunning(true), fileHandle(nullptr), consoleHandle(nullptr)
			{
				//设置locale为.UTF-8用于多语言支持
				std::locale::global(std::locale(".UTF-8"));

				fileHandle = CreateFileA("log.txt", GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, nullptr);

				if (INVALID_HANDLE_VALUE == fileHandle)
				{
					fileHandle = nullptr;
				}

				consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);

				if (INVALID_HANDLE_VALUE == consoleHandle)
				{
					consoleHandle = nullptr;
				}

				if (consoleHandle)
				{
					SetConsoleOutputCP(CP_UTF8);

					DWORD consoleMode = 0;

					GetConsoleMode(consoleHandle, &consoleMode);

					consoleMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;

					SetConsoleMode(consoleHandle, consoleMode);
				}

				worker = std::thread(&LoggerImpl::workerLoop, this);
			}

			~LoggerImpl()
			{
				shutdown();

				if (fileHandle)
				{
					CloseHandle(fileHandle);

					fileHandle = nullptr;
				}
			}

			void submitLogMessage(const LogMessage& msg)
			{
				{
					std::lock_guard<std::mutex> lockGuard(queueLock);

					messages.push(msg);
				}

				cv.notify_one();
			}

		private:

			std::queue<LogMessage> messages;

			bool isRunning;

			std::mutex queueLock;

			std::condition_variable cv;

			std::string temp;

			std::thread worker;

			HANDLE fileHandle;

			HANDLE consoleHandle;

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
					LogMessage message = std::move(messages.front());

					messages.pop();

					temp = message.str;

					{
						std::lock_guard<std::mutex> inUseLock(message.inUseMutex);

						message.readIndex++;
					}

					message.inUseCV.notify_one();

					if (fileHandle)
					{
						DWORD bytesWritten = 0;

						WriteFile(fileHandle, temp.c_str(), static_cast<DWORD>(temp.size()), &bytesWritten, nullptr);
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
						LogMessage message = std::move(messages.front());

						messages.pop();

						lock.unlock();

						temp = message.str;

						{
							std::lock_guard<std::mutex> inUseLock(message.inUseMutex);

							message.readIndex++;
						}

						message.inUseCV.notify_one();

						//ERROR类型在Gear::failureExit处被统一处理
						if (consoleHandle && message.type != LogType::LOG_ERROR)
						{
							WriteConsoleA(consoleHandle, temp.c_str(), static_cast<DWORD>(temp.size()), nullptr, nullptr);
						}

						if (fileHandle)
						{
							DWORD bytesWritten = 0;

							WriteFile(fileHandle, temp.c_str(), static_cast<DWORD>(temp.size()), &bytesWritten, nullptr);
						}

						lock.lock();
					}
				}
			}
		};

		UniquePtr<LoggerImpl> impl;

		void initialize()
		{
			impl = makeUnique<LoggerImpl>();
		}

		void release()
		{
			impl.reset();
		}
	}

	void submitLogMessage(const LogMessage& msg)
	{
		Internal::impl->submitLogMessage(msg);
	}
}
