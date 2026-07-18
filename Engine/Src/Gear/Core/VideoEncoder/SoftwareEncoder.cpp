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

		codecContext->gop_size = static_cast<int>(gopLength);

		codecContext->color_primaries = AVCOL_PRI_BT709;

		codecContext->color_trc = AVCOL_TRC_IEC61966_2_1;

		codecContext->colorspace = AVCOL_SPC_BT709;

		codecContext->color_range = AVCOL_RANGE_MPEG;

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

		cpCommandList = makeUnique<D3D12Core::GraphicsCommandList>(D3D12_COMMAND_LIST_TYPE_COPY);

		cpCommandQueue = makeUnique<D3D12Core::CommandQueue>(cpCommandList.get());
	}

	SoftwareEncoder::~SoftwareEncoder()
	{
		avcodec_send_frame(codecContext, nullptr);

		while (avcodec_receive_packet(codecContext, packet) == 0)
		{
			av_packet_unref(packet);
		}

		av_packet_free(&packet);

		av_frame_free(&yuvFrame);

		sws_freeContext(swsContext);

		avcodec_free_context(&codecContext);
	}

	bool SoftwareEncoder::encode(D3D12Resource::Texture* const inputTexture)
	{
		if (!readbackHeap)
		{
			readbackHeap = makeUnique<D3D12Resource::ReadbackHeap>(inputTexture->getWidth() * inputTexture->getHeight() * FMT::getByteSize(Graphics::backBufferFormat));
		}

		cpCommandQueue->begin();

		{
			D3D12_PLACED_SUBRESOURCE_FOOTPRINT bufferFootprint = {};

			bufferFootprint.Footprint.Width = inputTexture->getWidth();

			bufferFootprint.Footprint.Height = inputTexture->getHeight();

			bufferFootprint.Footprint.Depth = 1;

			bufferFootprint.Footprint.RowPitch = FMT::getByteSize(Graphics::backBufferFormat) * inputTexture->getWidth();

			bufferFootprint.Footprint.Format = Graphics::backBufferFormat;

			const CD3DX12_TEXTURE_COPY_LOCATION copyDest(readbackHeap->getResource(), bufferFootprint);

			const CD3DX12_TEXTURE_COPY_LOCATION copySrc(inputTexture->getResource(), 0);

			cpCommandList->trackAndSetResourceState(inputTexture, D3D12Resource::D3D12_TRANSITION_ALL_MIPLEVELS, D3D12_RESOURCE_STATE_COPY_SOURCE);

			cpCommandList->flushResourceBarriers();

			cpCommandList->get()->CopyTextureRegion(&copyDest, 0, 0, 0, &copySrc, nullptr);

			cpCommandList->trackAndSetResourceState(inputTexture, D3D12Resource::D3D12_TRANSITION_ALL_MIPLEVELS, D3D12_RESOURCE_STATE_COMMON);

			cpCommandList->flushResourceBarriers();
		}

		cpCommandQueue->processCommandLists();

		cpCommandQueue->waitFrameGPUComplete();

		const uint8_t* const data = reinterpret_cast<const uint8_t*>(readbackHeap->map());

		const uint8_t* sourceData[] = { data };

		const int32_t sourceStride[] = { static_cast<int32_t>(codecContext->width) * static_cast<int32_t>(FMT::getByteSize(Graphics::backBufferFormat)) };

		sws_scale(swsContext, sourceData, sourceStride, 0, codecContext->height, yuvFrame->data, yuvFrame->linesize);

		//强制尾帧为P帧（编译器有可能决定其为I帧，但不可能为B帧）
		//因为libx264可能不会以编码顺序输出编码后的比特流
		//另外，这也可以支持循环动画
		yuvFrame->pict_type = ((Graphics::getRenderedFrameCount() == frameToEncode - 1u) ? AV_PICTURE_TYPE_P : AV_PICTURE_TYPE_NONE);

		//YUV帧的呈现时间戳应该等于已渲染帧数
		yuvFrame->pts = static_cast<int64_t>(Graphics::getRenderedFrameCount());

		avcodec_send_frame(codecContext, yuvFrame);

		while (avcodec_receive_packet(codecContext, packet) == 0)
		{
			//抛弃在视频时间轴之外的packet
			//这么做需要强制末尾几帧为P或I帧
			if (packet->pts < static_cast<int64_t>(frameToEncode))
			{
				av_packet_rescale_ts(packet, codecContext->time_base, getOutStream()->time_base);

				packet->duration = av_rescale_q(1, AVRational{ 1, static_cast<int32_t>(frameRate) }, getOutStream()->time_base);

				packet->stream_index = getOutStream()->index;

				if (!writeFrame(packet))
				{
					av_packet_unref(packet);

					readbackHeap->unmap();

					return false;
				}
			}

			av_packet_unref(packet);
		}

		readbackHeap->unmap();

		return true;
	}
}
