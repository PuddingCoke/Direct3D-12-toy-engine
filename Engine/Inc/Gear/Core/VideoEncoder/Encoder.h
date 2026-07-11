#pragma once

#ifndef _GEAR_CORE_VIDEOENCODER_ENCODER_H_
#define _GEAR_CORE_VIDEOENCODER_ENCODER_H_

#include<Gear/Core/Graphics.h>

#include<Gear/Core/VideoDevice.h>

#include<Gear/Core/D3D12Resource/Texture.h>

#include<Gear/Core/D3D12Resource/VideoTexture.h>

#include<Gear/Core/D3D12Core/VideoProcessCommandList.h>

#include<Gear/Core/D3D12Core/CommandQueue.h>

#include<chrono>

extern "C"
{
#include<ffmpeg/libavutil/avutil.h>
#include<ffmpeg/libavformat/avformat.h>
#include<ffmpeg/libavcodec/avcodec.h>
}

namespace Gear::Core::VideoEncoder
{
	class Encoder
	{
	public:

		enum class VideoFormat
		{
			H264, HEVC, AV1
		};

		Encoder() = delete;

		Encoder(const Encoder&) = delete;

		void operator=(const Encoder&) = delete;

		Encoder(const uint32_t frameToEncode, const VideoFormat videoFormat, const uint32_t maxBFrames);

		virtual ~Encoder();

		virtual bool encode(D3D12Resource::Texture* const inputTexture) = 0;

		static constexpr uint32_t frameRate = 60;

		void waitFor(D3D12Core::CommandQueue* const queueWaitFor, D3D12Core::Fence* const fence);

	protected:

		/// <summary>
		/// 封装裸的比特流
		/// </summary>
		/// <param name="bitstreamPtr">比特流指针</param>
		/// <param name="bitstreamSize">比特流字节大小</param>
		/// <param name="cleanPoint">同步点 pictureType == IDR 用于视频的正确跳转</param>
		/// <param name="pts">呈现时间戳 0、1、2、3.....</param>
		/// <returns></returns>
		bool writeFrame(void* const bitstreamPtr, const uint32_t bitstreamSize, const bool syncPoint, const uint32_t presentFrameIndex);

		void bgraToNV12(D3D12Resource::Texture* inputTexture, D3D12Resource::VideoTexture* nv12Texture, D3D12Core::Fence* const fence);

	private:

		void displayProgress() const;

		//不要修改这个值
		static constexpr uint32_t progressBarWidth = 32;

		uint32_t frameEncoded;

		const uint32_t frameToEncode;

		std::chrono::steady_clock::time_point startPoint;

		std::chrono::steady_clock::time_point endPoint;

		float encodeTime;

		const uint32_t maxBFrames;

		AVFormatContext* outContext;

		AVStream* outStream;

		ComPtr<ID3D12VideoProcessor> videoProcessor;

		D3D12Core::VideoProcessCommandListPtr vpCommandList;

		D3D12Core::CommandQueuePtr vpCommandQueue;

	};
}

#endif // !_GEAR_CORE_VIDEOENCODER_ENCODER_H_
