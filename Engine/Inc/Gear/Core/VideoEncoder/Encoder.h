﻿#pragma once

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

	void displayProgress() const;

	//封装比特流
	bool writeFrame(const void* const bitstreamPtr, const uint32_t bitstreamSize, const bool cleanPoint);

private:

	//不要修改这个值
	static constexpr uint32_t progressBarWidth = 32;

	uint32_t frameEncoded;

	const uint32_t frameToEncode;

	std::chrono::steady_clock::time_point startPoint;

	std::chrono::steady_clock::time_point endPoint;

	float encodeTime;

	ComPtr<IMFSinkWriter> sinkWriter;

	DWORD streamIndex;

	const LONGLONG sampleDuration;

	LONGLONG sampleTime;

};

#endif // !_ENCODER_H_