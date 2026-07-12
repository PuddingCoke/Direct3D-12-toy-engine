#include<Gear/Core/VideoEncoder/SoftwareEncoder.h>

namespace Gear::Core::VideoEncoder
{
	SoftwareEncoder::SoftwareEncoder(const uint32_t frameToEncode) :
		Encoder(frameToEncode, videoFormat)
	{
		const AVCodec* codec = avcodec_find_encoder_by_name("libx264");

		codecContext = avcodec_alloc_context3(codec);

		codecContext->width = Graphics::getWidth();

		codecContext->height = Graphics::getHeight();

		codecContext->time_base = AVRational{ 1, static_cast<int>(frameRate) };

		codecContext->framerate = AVRational{ static_cast<int32_t>(frameRate),1 };

		codecContext->pix_fmt = destFormat;

		codecContext->gop_size = static_cast<int>(frameRate);

		AVDictionary* opts = nullptr;

		av_dict_set(&opts, "preset", "veryslow", 0);

		av_dict_set(&opts, "crf", "18", 0);

		avcodec_open2(codecContext, codec, &opts);

		av_dict_free(&opts);

		avcodec_parameters_from_context(getOutStream()->codecpar, codecContext);

		swsContext = sws_getContext(codecContext->width, codecContext->height, sourceFormat,
			codecContext->width, codecContext->height, destFormat, SWS_BILINEAR, nullptr, nullptr, nullptr);

		yuvFrame = av_frame_alloc();

		yuvFrame->format = codecContext->pix_fmt;

		yuvFrame->width = codecContext->width;

		yuvFrame->height = codecContext->height;

		av_frame_get_buffer(yuvFrame, 0);

		packet = av_packet_alloc();

		writeHeader();
	}

	SoftwareEncoder::~SoftwareEncoder()
	{
		/*待实现*/
	}

	bool SoftwareEncoder::encode(const uint8_t* const data)
	{
		const uint8_t* sourceData[] = { data };

		const int32_t sourceStride[] = { static_cast<int32_t>(codecContext->width) * static_cast<int32_t>(FMT::getByteSize(Graphics::backBufferFormat)) };

		sws_scale(swsContext, sourceData, sourceStride, 0, codecContext->height, yuvFrame->data, yuvFrame->linesize);

		yuvFrame->pts = static_cast<int64_t>(Graphics::getRenderedFrameCount());

		avcodec_send_frame(codecContext, yuvFrame);

		while (avcodec_receive_packet(codecContext, packet) == 0)
		{
			if (!writeFrame(packet))
			{
				return false;
			}
		}

		return true;
	}
}
