#include<Gear/Core/VideoEncoder/Encoder.h>

Encoder::Encoder(const uint32_t frameToEncode) :
	frameToEncode(frameToEncode), frameEncoded(0), encoding(true), encodeTime(0.f)
{
}

Encoder::~Encoder()
{
}

void Encoder::displayResult() const
{
	LOGENGINE("encode complete");

	LOGENGINE("encode average speed", frameToEncode / encodeTime);
}
