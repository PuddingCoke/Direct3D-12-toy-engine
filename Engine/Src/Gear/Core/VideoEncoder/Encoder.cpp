#include<Gear/Core/VideoEncoder/Encoder.h>

Encoder::Encoder(const uint32_t frameToEncode) :
	frameToEncode(frameToEncode), frameEncoded(0), encoding(true), encodeTime(0.f), hConsole(GetStdHandle(STD_OUTPUT_HANDLE))
{
}

Encoder::~Encoder()
{
}

void Encoder::displayProgress() const
{
	if (encoding)
	{
		CONSOLE_SCREEN_BUFFER_INFO csbi = {};

		GetConsoleScreenBufferInfo(hConsole, &csbi);

		const uint32_t num = progressBarWidth * frameEncoded / frameToEncode;

		std::cout << "Encoding... [" << std::string(num, '*') << std::string(progressBarWidth - num, '/') << "] " << std::fixed << std::setprecision(2) << 100.f * static_cast<float>(frameEncoded) / static_cast<float>(frameToEncode) << "%";

		SetConsoleCursorPosition(hConsole, csbi.dwCursorPosition);
	}
}
