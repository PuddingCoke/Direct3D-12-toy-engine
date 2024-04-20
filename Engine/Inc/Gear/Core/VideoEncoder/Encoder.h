#pragma once

#ifndef _ENCODER_H_
#define _ENCODER_H_

#include<chrono>
#include<iomanip>

#include<Gear/Core/Graphics.h>

#include<Gear/Core/DX/Resource/Texture.h>

class Encoder
{
public:

	Encoder() = delete;

	Encoder(const Encoder&) = delete;

	void operator=(const Encoder&) = delete;

	Encoder(const UINT frameToEncode);

	virtual ~Encoder();

	virtual bool encode(Texture* const inputTexture) = 0;

protected:

	static constexpr UINT frameRate = 60;

	static constexpr UINT progressBarWidth = 30;

	void displayProgress() const;

	UINT frameEncoded;

	const UINT frameToEncode;

	bool encoding;

	std::chrono::steady_clock timer;

	std::chrono::steady_clock::time_point timeStart;

	std::chrono::steady_clock::time_point timeEnd;

	float encodeTime;

private:

	HANDLE hConsole;

};

#endif // !_ENCODER_H_
