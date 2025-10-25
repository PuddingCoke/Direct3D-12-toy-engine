#include<Gear/Utils/Logger/LogColor.h>

bool Utils::Logger::LogColor::operator!=(const LogColor& color) const
{
	return code != color.code;
}

const Utils::Logger::LogColor Utils::Logger::LogColor::black = { L"\033[30m" };

const Utils::Logger::LogColor Utils::Logger::LogColor::red = { L"\033[31m" };

const Utils::Logger::LogColor Utils::Logger::LogColor::green = { L"\033[32m" };

const Utils::Logger::LogColor Utils::Logger::LogColor::yellow = { L"\033[33m" };

const Utils::Logger::LogColor Utils::Logger::LogColor::blue = { L"\033[34m" };

const Utils::Logger::LogColor Utils::Logger::LogColor::magenta = { L"\033[35m" };

const Utils::Logger::LogColor Utils::Logger::LogColor::cyan = { L"\033[36m" };

const Utils::Logger::LogColor Utils::Logger::LogColor::white = { L"\033[37m" };

const Utils::Logger::LogColor Utils::Logger::LogColor::brightBlack = { L"\033[90m" };

const Utils::Logger::LogColor Utils::Logger::LogColor::brightRed = { L"\033[91m" };

const Utils::Logger::LogColor Utils::Logger::LogColor::brightGreen = { L"\033[92m" };

const Utils::Logger::LogColor Utils::Logger::LogColor::brightYellow = { L"\033[93m" };

const Utils::Logger::LogColor Utils::Logger::LogColor::brightBlue = { L"\033[94m" };

const Utils::Logger::LogColor Utils::Logger::LogColor::brightMagenta = { L"\033[95m" };

const Utils::Logger::LogColor Utils::Logger::LogColor::brightCyan = { L"\033[96m" };

const Utils::Logger::LogColor Utils::Logger::LogColor::brightWhite = { L"\033[97m" };

const Utils::Logger::LogColor Utils::Logger::LogColor::defaultColor = Utils::Logger::LogColor::brightWhite;

const Utils::Logger::LogColor Utils::Logger::LogColor::numericColor = Utils::Logger::LogColor::brightCyan;

const Utils::Logger::LogColor Utils::Logger::LogColor::timeStampColor = Utils::Logger::LogColor::brightBlack;

const Utils::Logger::LogColor Utils::Logger::LogColor::threadIdColor = Utils::Logger::LogColor::cyan;

const Utils::Logger::LogColor Utils::Logger::LogColor::functionNameColor = Utils::Logger::LogColor::yellow;

const Utils::Logger::LogColor Utils::Logger::LogColor::successColor = Utils::Logger::LogColor::brightGreen;

const Utils::Logger::LogColor Utils::Logger::LogColor::errorColor = Utils::Logger::LogColor::brightRed;

const Utils::Logger::LogColor Utils::Logger::LogColor::engineColor = Utils::Logger::LogColor::brightYellow;

const Utils::Logger::LogColor Utils::Logger::LogColor::userColor = Utils::Logger::LogColor::brightWhite;
