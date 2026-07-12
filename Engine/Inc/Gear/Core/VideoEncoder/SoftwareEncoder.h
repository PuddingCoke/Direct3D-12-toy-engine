#pragma once

#ifndef _GEAR_CORE_VIDEOENCODER_SOFTWAREENCODER_H_
#define _GEAR_CORE_VIDEOENCODER_SOFTWAREENCODER_H_

#include"Encoder.h"

extern "C"
{
#include<ffmpeg/libswscale/swscale.h>
}

namespace Gear::Core::VideoEncoder
{
	//施工中
	class SoftwareEncoder :public Encoder
	{
	public:

		SoftwareEncoder(const uint32_t frameToEncode);

		~SoftwareEncoder();

		bool encode(const uint8_t* const data) override;

	private:

		static constexpr VideoFormat videoFormat = VideoFormat::H264;

		static constexpr AVCodecID codecID = AV_CODEC_ID_H264;

		static constexpr AVPixelFormat sourceFormat = AV_PIX_FMT_BGRA;

		static constexpr AVPixelFormat destFormat = AV_PIX_FMT_YUV420P;

		AVCodecContext* codecContext = nullptr;

		AVFrame* yuvFrame = nullptr;

		SwsContext* swsContext = nullptr;

		AVPacket* packet = nullptr;

	};
}

#endif // !_GEAR_CORE_VIDEOENCODER_SOFTWAREENCODER_H_
