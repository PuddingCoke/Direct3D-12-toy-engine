#include<Gear/Core/VideoEncoder/Encoder.h>

Encoder::Encoder(const UINT frameToEncode) :
	frameToEncode(frameToEncode), frameEncoded(0), encoding(true), encodeTime(0.f), hConsole(GetStdHandle(STD_OUTPUT_HANDLE))
{
}

Encoder::~Encoder()
{
}

void Encoder::displayProgress()
{
	CONSOLE_SCREEN_BUFFER_INFO csbi = {};

	GetConsoleScreenBufferInfo(hConsole, &csbi);

	std::cout << "Encoding... [";

	const UINT num = progressBarWidth * (frameEncoded + 1) / frameToEncode;

	std::cout << std::string(num, '*') << std::string(progressBarWidth - num, '/');

	std::cout << "] " << std::fixed << std::setprecision(2) << 100.f * static_cast<float>(frameEncoded + 1) / static_cast<float>(frameToEncode) << "%";

	SetConsoleCursorPosition(hConsole, csbi.dwCursorPosition);
}
