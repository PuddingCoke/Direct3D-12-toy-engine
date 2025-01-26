#pragma once

#ifndef _ENCODER_H_
#define _ENCODER_H_

#include<Gear/Core/Graphics.h>

#include<Gear/Core/DX/Texture.h>

#include<chrono>

class Encoder
{
public:

	Encoder() = delete;

	Encoder(const Encoder&) = delete;

	void operator=(const Encoder&) = delete;

	Encoder(const uint32_t frameToEncode);

	virtual ~Encoder();

	virtual bool encode(Texture* const inputTexture) = 0;

	static constexpr uint32_t lookaheadDepth = 31;

	static constexpr uint32_t frameRate = 60;

protected:

	void displayResult() const;

	uint32_t frameEncoded;

	const uint32_t frameToEncode;

	bool encoding;

	std::chrono::steady_clock::time_point timeStart;

	std::chrono::steady_clock::time_point timeEnd;

	float encodeTime;

};

#endif // !_ENCODER_H_