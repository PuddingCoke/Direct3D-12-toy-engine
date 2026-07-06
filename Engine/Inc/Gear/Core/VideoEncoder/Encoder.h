#pragma once

#ifndef _GEAR_CORE_VIDEOENCODER_ENCODER_H_
#define _GEAR_CORE_VIDEOENCODER_ENCODER_H_

#include<mfapi.h>
#include<mfidl.h>
#include<mfreadwrite.h>

#include<Gear/Core/Graphics.h>

#include<Gear/Core/VideoDevice.h>

#include<Gear/Core/D3D12Resource/Texture.h>

#include<Gear/Core/D3D12Resource/VideoTexture.h>

#include<Gear/Core/D3D12Core/VideoProcessCommandList.h>

#include<Gear/Core/D3D12Core/CommandQueue.h>

#include<chrono>

namespace Gear::Core::VideoEncoder
{
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

		virtual bool encode(D3D12Resource::Texture* const inputTexture) = 0;

		static constexpr uint32_t frameRate = 60;

		void waitFor(D3D12Core::CommandQueue* const queueWaitFor, D3D12Core::Fence* const fence);

	protected:

		//封装比特流
		bool writeFrame(const void* const bitstreamPtr, const uint32_t bitstreamSize, const bool cleanPoint);

		void bgraToNV12(D3D12Resource::Texture* inputTexture, D3D12Resource::VideoTexture* nv12Texture, D3D12Core::Fence* const fence);

		ComPtr<ID3D12VideoProcessor> videoProcessor;

		D3D12Core::VideoProcessCommandListPtr vpCommandList;

		D3D12Core::CommandQueuePtr vpCommandQueue;

	private:

		void displayProgress() const;

		//不要修改这个值
		static constexpr uint32_t progressBarWidth = 32;

		uint32_t frameEncoded;

		const uint32_t frameToEncode;

		std::chrono::steady_clock::time_point startPoint;

		std::chrono::steady_clock::time_point endPoint;

		float encodeTime;

		DWORD streamIndex;

		ComPtr<IMFSinkWriter> sinkWriter;

		const LONGLONG sampleDuration;

		LONGLONG sampleTime;

	};
}

#endif // !_GEAR_CORE_VIDEOENCODER_ENCODER_H_
