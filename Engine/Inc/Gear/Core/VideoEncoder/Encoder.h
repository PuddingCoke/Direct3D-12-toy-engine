#pragma once

#ifndef _ENCODER_H_
#define _ENCODER_H_

#include<mfapi.h>
#include<mfidl.h>
#include<mfreadwrite.h>

#include<Gear/Core/Graphics.h>

#include<Gear/Core/Resource/D3D12Resource/Texture.h>

#include<chrono>

class Encoder
{
public:

	enum class OutputVideoFormat
	{
		H264, HEVC, AV1
	};

	Encoder() = delete;

	Encoder(const Encoder&) = delete;

	void operator=(const Encoder&) = delete;

	Encoder(const uint32_t frameToEncode, const OutputVideoFormat format);

	virtual ~Encoder();

	virtual bool encode(Texture* const inputTexture) = 0;

	static constexpr uint32_t frameRate = 60;

protected:

	void displayResult() const;

	//封装比特流
	bool writeFrame(const void* const bitstreamPtr, const uint32_t bitstreamSize, const bool cleanPoint);

	//计时（调用两次记录一次编码的时间）
	void tick();

private:

	uint32_t frameEncoded;

	const uint32_t frameToEncode;

	std::chrono::steady_clock::time_point timeStart;

	std::chrono::steady_clock::time_point timeEnd;

	bool isStartTimePoint;

	float encodeTime;

	ComPtr<IMFSinkWriter> sinkWriter;

	DWORD streamIndex;

	const LONGLONG sampleDuration;

	LONGLONG sampleTime;

};

#endif // !_ENCODER_H_