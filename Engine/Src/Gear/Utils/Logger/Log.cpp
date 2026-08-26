#include<Gear/Utils/Logger/Log.h>

#include<Gear/Utils/Logger.h>

#include<stdexcept>

namespace Gear::Utils::Logger
{
	Log::Log(const std::string_view& functionName, const LogType& type) :
		context(LogContext::get())
	{
		context.createLogMessage(functionName, type);
	}

	Log::~Log()
	{
		finishAndPostMessage();
	}

	const Log& Log::operator<<(const std::wstring& arg) const
	{
		context.packArgument(arg);

		return *this;
	}

	const Log& Log::operator<<(const wchar_t* arg) const
	{
		context.packArgument(arg);

		return *this;
	}

	const Log& Log::operator<<(const std::string& arg) const
	{
		context.packArgument(arg);

		return *this;
	}

	const Log& Log::operator<<(const char* arg) const
	{
		context.packArgument(arg);

		return *this;
	}

	const Log& Log::operator<<(const std::u8string& arg) const
	{
		context.packArgument(arg);

		return *this;
	}

	const Log& Log::operator<<(const char8_t* arg) const
	{
		context.packArgument(arg);

		return *this;
	}

	const Log& Log::operator<<(const bool& arg) const
	{
		context.packArgument(arg);

		return *this;
	}

	const Log& Log::operator<<(const int32_t& arg) const
	{
		context.packArgument(arg);

		return *this;
	}

	const Log& Log::operator<<(const int64_t& arg) const
	{
		context.packArgument(arg);

		return *this;
	}

	const Log& Log::operator<<(const uint32_t& arg) const
	{
		context.packArgument(arg);

		return *this;
	}

	const Log& Log::operator<<(const uint64_t& arg) const
	{
		context.packArgument(arg);

		return *this;
	}

	const Log& Log::operator<<(const float_t& arg) const
	{
		context.packArgument(arg);

		return *this;
	}

	const Log& Log::operator<<(const double_t& arg) const
	{
		context.packArgument(arg);

		return *this;
	}

	const Log& Log::operator<<(const LogIntegerMode& arg) const
	{
		context.packArgument(arg);

		return *this;
	}

	const Log& Log::operator<<(const LogFloatPrecision& arg) const
	{
		context.packArgument(arg);

		return *this;
	}

	const Log& Log::operator<<(const LogColor& arg) const
	{
		context.packArgument(arg);

		return *this;
	}

	bool Log::needThrow() const
	{
		return context.getLogType() == LogType::LOG_ERROR;
	}

	void Log::finishAndPostMessage() const
	{
		if (!needThrow())
		{
			context.packArgument(LogContext::NewLine());

			Logger::submitLogMessage(context.getLogMessage());
		}
	}

	void Log::finishAndThrowMessage() const
	{
		if (needThrow())
		{
			context.packArgument(LogContext::NewLine());

			const LogMessage message = context.getLogMessage();

			const std::string errorStr = message.str;

			Logger::submitLogMessage(message);

            throw std::runtime_error(errorStr);
		}
	}

	void ThrowLog(const Log& log)
	{
		log.finishAndThrowMessage();
	}
}
