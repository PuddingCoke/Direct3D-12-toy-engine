#include<Gear/Core/VideoEncoder/Encoder.h>

namespace Gear::Core::VideoEncoder
{
	Encoder::Encoder(const uint32_t frameToEncode, const VideoFormat videoFormat) :
		frameEncoded(0), frameToEncode(frameToEncode), encodeTime(0.f), streamIndex(0), sampleDuration(timeBase / static_cast<LONGLONG>(frameRate)), dts(0)
	{
		CHECKERROR(MFStartup(MF_VERSION));

		CHECKERROR(MFCreateSinkWriterFromURL(L"output.mp4", nullptr, nullptr, &sinkWriter));

		ComPtr<IMFMediaType> mediaType;

		CHECKERROR(MFCreateMediaType(&mediaType));

		mediaType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Video);

		switch (videoFormat)
		{
		case VideoFormat::H264:
			LOGENGINE("视频格式", LogColor::brightMagenta, "H264");
			mediaType->SetGUID(MF_MT_SUBTYPE, MFVideoFormat_H264);
			break;
		case VideoFormat::HEVC:
			LOGENGINE("视频格式", LogColor::brightMagenta, "HEVC");
			mediaType->SetGUID(MF_MT_SUBTYPE, MFVideoFormat_HEVC);
			break;
		case VideoFormat::AV1:
			LOGENGINE("视频格式", LogColor::brightMagenta, "AV1");
			mediaType->SetGUID(MF_MT_SUBTYPE, MFVideoFormat_AV1);
			break;
		default:
			LOGERROR("不被支持的视频格式！");
			break;
		}

		LOGENGINE("视频时间", FloatPrecision(1), static_cast<float>(frameToEncode) / static_cast<float>(frameRate), "秒");

		LOGENGINE("视频帧率", frameRate);

		LOGENGINE("待编码帧数", frameToEncode);

		MFSetAttributeSize(mediaType.Get(), MF_MT_FRAME_SIZE, Graphics::getWidth(), Graphics::getHeight());

		MFSetAttributeRatio(mediaType.Get(), MF_MT_FRAME_RATE, frameRate, 1);

		sinkWriter->AddStream(mediaType.Get(), &streamIndex);

		sinkWriter->BeginWriting();

		D3D12_VIDEO_PROCESS_INPUT_STREAM_DESC inputDesc =
		{
			.Format = Graphics::backBufferFormat,
			.ColorSpace = DXGI_COLOR_SPACE_RGB_FULL_G22_NONE_P709,
			.FrameRate = {frameRate, 1},
			.SourceSizeRange = {Graphics::getWidth(), Graphics::getHeight(), Graphics::getWidth(), Graphics::getHeight()},
			.DestinationSizeRange = {Graphics::getWidth(), Graphics::getHeight(), Graphics::getWidth(), Graphics::getHeight()},
		};

		D3D12_VIDEO_PROCESS_OUTPUT_STREAM_DESC outputDesc =
		{
		.Format = DXGI_FORMAT_NV12,
		.ColorSpace = DXGI_COLOR_SPACE_YCBCR_STUDIO_G22_LEFT_P709,
		.FrameRate = {frameRate, 1},
		};

		CHECKERROR(VideoDevice::get()->CreateVideoProcessor(0, &outputDesc, 1, &inputDesc, IID_PPV_ARGS(&videoProcessor)));

		vpCommandList = makeUnique<D3D12Core::VideoProcessCommandList>();

		vpCommandQueue = makeUnique<D3D12Core::CommandQueue>(D3D12_COMMAND_LIST_TYPE_VIDEO_PROCESS);

		vpCommandQueue->setPrepareCommandList(vpCommandList.get());
	}

	Encoder::~Encoder()
	{
		if (vpCommandQueue)
		{
			vpCommandQueue->waitDestroyable();
		}

		if (sinkWriter)
		{
			sinkWriter->Finalize();

			sinkWriter = nullptr;
		}

		MFShutdown();
	}

	void Encoder::waitFor(D3D12Core::CommandQueue* const queueWaitFor, D3D12Core::Fence* const fence)
	{
		vpCommandQueue->waitFor(queueWaitFor, fence);
	}

	bool Encoder::writeFrame(const void* const bitstreamPtr, const uint32_t bitstreamSize, const bool cleanPoint, const LONGLONG pts)
	{
		ComPtr<IMFMediaBuffer> buffer;

		MFCreateMemoryBuffer(bitstreamSize, &buffer);

		BYTE* data = nullptr;

		buffer->Lock(&data, nullptr, nullptr);

		memcpy(data, bitstreamPtr, bitstreamSize);

		buffer->Unlock();

		buffer->SetCurrentLength(bitstreamSize);

		ComPtr<IMFSample> sample;

		MFCreateSample(&sample);

		sample->AddBuffer(buffer.Get());

		//pts
		sample->SetSampleTime(pts * sampleDuration);

		//dts
		sample->SetUINT64(MFSampleExtension_DecodeTimestamp, static_cast<uint64_t>(dts * sampleDuration));

		//duration
		sample->SetSampleDuration(sampleDuration);

		dts++;

		if (cleanPoint)
		{
			sample->SetUINT32(MFSampleExtension_CleanPoint, TRUE);
		}

		sinkWriter->WriteSample(streamIndex, sample.Get());

		frameEncoded++;

		displayProgress();

		return !(frameEncoded == frameToEncode);
	}

	void Encoder::bgraToNV12(D3D12Resource::Texture* inputTexture, D3D12Resource::VideoTexture* nv12Texture, D3D12Core::Fence* const fence)
	{
		vpCommandQueue->begin();

		D3D12Core::VPInputArguments inputArgs = D3D12Core::VPInputArguments(inputTexture);

		D3D12Core::VPOutputArguments outputArgs = D3D12Core::VPOutputArguments(nv12Texture);

		vpCommandList->processFrames(videoProcessor.Get(), outputArgs, { inputArgs });

		vpCommandQueue->processCommandLists();

		vpCommandQueue->waitFrameGPUComplete();

		vpCommandQueue->signal(fence);
	}

	void Encoder::displayProgress() const
	{
		if ((frameEncoded % (frameRate / 4)) == 0)
		{
			const float progress = Utils::Math::saturate(static_cast<float>(frameEncoded) / static_cast<float>(frameToEncode));

			const uint32_t num = static_cast<uint32_t>(std::max(std::min(static_cast<int32_t>(progressBarWidth * progress), static_cast<int32_t>(progressBarWidth)), 0));

			const uint32_t buffLength = 13 + 2 + progressBarWidth + 1 + 6 + 1 + 1 + 8;

			char str[buffLength] = {};

			sprintf_s(str, buffLength, "编码中... [%.*s%.*s] %.2f%%",
				num, "********************************",
				progressBarWidth - num, "////////////////////////////////",
				100.f * progress);

			LOGENGINE(str);
		}
	}
}
