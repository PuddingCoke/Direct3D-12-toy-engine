#include<Gear/Utils/Logger/LogColor.h>

bool Gear::Utils::Logger::LogColor::operator!=(const LogColor& color) const
{
	return code != color.code;
}

const Gear::Utils::Logger::LogColor Gear::Utils::Logger::LogColor::black = { L"\033[30m" };

const Gear::Utils::Logger::LogColor Gear::Utils::Logger::LogColor::red = { L"\033[31m" };

const Gear::Utils::Logger::LogColor Gear::Utils::Logger::LogColor::green = { L"\033[32m" };

const Gear::Utils::Logger::LogColor Gear::Utils::Logger::LogColor::yellow = { L"\033[33m" };

const Gear::Utils::Logger::LogColor Gear::Utils::Logger::LogColor::blue = { L"\033[34m" };

const Gear::Utils::Logger::LogColor Gear::Utils::Logger::LogColor::magenta = { L"\033[35m" };

const Gear::Utils::Logger::LogColor Gear::Utils::Logger::LogColor::cyan = { L"\033[36m" };

const Gear::Utils::Logger::LogColor Gear::Utils::Logger::LogColor::white = { L"\033[37m" };

const Gear::Utils::Logger::LogColor Gear::Utils::Logger::LogColor::brightBlack = { L"\033[90m" };

const Gear::Utils::Logger::LogColor Gear::Utils::Logger::LogColor::brightRed = { L"\033[91m" };

const Gear::Utils::Logger::LogColor Gear::Utils::Logger::LogColor::brightGreen = { L"\033[92m" };

const Gear::Utils::Logger::LogColor Gear::Utils::Logger::LogColor::brightYellow = { L"\033[93m" };

const Gear::Utils::Logger::LogColor Gear::Utils::Logger::LogColor::brightBlue = { L"\033[94m" };

const Gear::Utils::Logger::LogColor Gear::Utils::Logger::LogColor::brightMagenta = { L"\033[95m" };

const Gear::Utils::Logger::LogColor Gear::Utils::Logger::LogColor::brightCyan = { L"\033[96m" };

const Gear::Utils::Logger::LogColor Gear::Utils::Logger::LogColor::brightWhite = { L"\033[97m" };

const Gear::Utils::Logger::LogColor Gear::Utils::Logger::LogColor::defaultColor = Gear::Utils::Logger::LogColor::brightWhite;

const Gear::Utils::Logger::LogColor Gear::Utils::Logger::LogColor::numericColor = Gear::Utils::Logger::LogColor::brightCyan;

const Gear::Utils::Logger::LogColor Gear::Utils::Logger::LogColor::timeStampColor = Gear::Utils::Logger::LogColor::brightBlack;

const Gear::Utils::Logger::LogColor Gear::Utils::Logger::LogColor::threadIdColor = Gear::Utils::Logger::LogColor::cyan;

const Gear::Utils::Logger::LogColor Gear::Utils::Logger::LogColor::functionNameColor = Gear::Utils::Logger::LogColor::yellow;

const Gear::Utils::Logger::LogColor Gear::Utils::Logger::LogColor::successColor = Gear::Utils::Logger::LogColor::brightGreen;

const Gear::Utils::Logger::LogColor Gear::Utils::Logger::LogColor::errorColor = Gear::Utils::Logger::LogColor::brightRed;

const Gear::Utils::Logger::LogColor Gear::Utils::Logger::LogColor::engineColor = Gear::Utils::Logger::LogColor::brightYellow;

const Gear::Utils::Logger::LogColor Gear::Utils::Logger::LogColor::userColor = Gear::Utils::Logger::LogColor::brightWhite;
