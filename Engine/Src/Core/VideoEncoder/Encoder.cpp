#include<Gear/Core/VideoEncoder/Encoder.h>

Encoder::Encoder(const uint32_t frameToEncode, const OutputVideoFormat format) :
	frameToEncode(frameToEncode), frameEncoded(0), isStartTimePoint(true), encodeTime(0.f), streamIndex(0), sampleDuration(10000000u / frameRate), sampleTime(0)
{
	CHECKERROR(MFStartup(MF_VERSION));

	CHECKERROR(MFCreateSinkWriterFromURL(L"output.mp4", nullptr, nullptr, &sinkWriter));

	ComPtr<IMFMediaType> mediaType;

	CHECKERROR(MFCreateMediaType(&mediaType));

	mediaType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Video);

	switch (format)
	{
	case OutputVideoFormat::H264:
		LOGENGINE("output video format", LogColor::brightMagenta, "H264");
		mediaType->SetGUID(MF_MT_SUBTYPE, MFVideoFormat_H264);
		break;
	case OutputVideoFormat::HEVC:
		LOGENGINE("output video format", LogColor::brightMagenta, "HEVC");
		mediaType->SetGUID(MF_MT_SUBTYPE, MFVideoFormat_HEVC);
		break;
	case OutputVideoFormat::AV1:
		LOGENGINE("output video format", LogColor::brightMagenta, "AV1");
		mediaType->SetGUID(MF_MT_SUBTYPE, MFVideoFormat_AV1);
		break;
	default:
		LOGERROR("not supported output video format!");
		break;
	}

	LOGENGINE("frame rate", frameRate);

	LOGENGINE("frame to encode", frameToEncode);

	MFSetAttributeSize(mediaType.Get(), MF_MT_FRAME_SIZE, Graphics::getWidth(), Graphics::getHeight());

	MFSetAttributeRatio(mediaType.Get(), MF_MT_FRAME_RATE, frameRate, 1);

	sinkWriter->AddStream(mediaType.Get(), &streamIndex);

	sinkWriter->BeginWriting();
}

Encoder::~Encoder()
{
	sinkWriter->Finalize();

	sinkWriter = nullptr;

	MFShutdown();
}

void Encoder::displayResult() const
{
	LOGENGINE("encode complete");

	LOGENGINE("encode average speed", frameToEncode / encodeTime);

	LOGENGINE("enter any key to quit");
}

bool Encoder::writeFrame(const void* const bitstreamPtr, const uint32_t bitstreamSize, const bool cleanPoint)
{
	frameEncoded++;

	sampleTime += sampleDuration;

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

	sample->SetSampleTime(sampleTime);

	sample->SetSampleDuration(sampleDuration);

	if (cleanPoint)
	{
		sample->SetUINT32(MFSampleExtension_CleanPoint, TRUE);
	}

	sinkWriter->WriteSample(streamIndex, sample.Get());

	return !(frameEncoded == frameToEncode);
}

void Encoder::tick()
{
	if (isStartTimePoint)
	{
		timeStart = std::chrono::steady_clock::now();
	}
	else
	{
		timeEnd = std::chrono::steady_clock::now();

		const float frameTime = std::chrono::duration<float>(timeEnd - timeStart).count();

		encodeTime += frameTime;
	}

	isStartTimePoint = !isStartTimePoint;
}
