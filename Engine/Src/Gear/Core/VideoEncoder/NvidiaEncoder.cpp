#include<Gear/Core/VideoEncoder/NvidiaEncoder.h>

NvidiaEncoder::NvidiaEncoder(const UINT frameToEncode) :
	Encoder(frameToEncode), encoder(nullptr),
	readbackHeap(new ReadbackHeap(2 * 4 * Graphics::getWidth() * Graphics::getHeight())),
	outCtx(nullptr), outStream(nullptr), pkt(nullptr),
	nvencAPI{ NV_ENCODE_API_FUNCTION_LIST_VER }, outputFenceValue(0)
{
	moduleNvEncAPI = LoadLibraryA("nvEncodeAPI64.dll");

	if (moduleNvEncAPI == 0)
	{
		throw "unable to load nvEncodeAPI64.dll";
	}

	GraphicsDevice::get()->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&outputFence));

	NVENCSTATUS(*ApiCreateInstance)(NV_ENCODE_API_FUNCTION_LIST*) = (NVENCSTATUS(*)(NV_ENCODE_API_FUNCTION_LIST*))GetProcAddress(moduleNvEncAPI, "NvEncodeAPICreateInstance");

	std::cout << "[class NvidiaEncoder] api instance create status " << ApiCreateInstance(&nvencAPI) << "\n";

	NV_ENC_OPEN_ENCODE_SESSION_EX_PARAMS sessionParams = { NV_ENC_OPEN_ENCODE_SESSION_EX_PARAMS_VER };
	sessionParams.device = GraphicsDevice::get();
	sessionParams.deviceType = NV_ENC_DEVICE_TYPE_DIRECTX;
	sessionParams.apiVersion = NVENCAPI_VERSION;

	NVENCCALL(nvencAPI.nvEncOpenEncodeSessionEx(&sessionParams, &encoder));

	NV_ENC_PRESET_CONFIG presetConfig = { NV_ENC_PRESET_CONFIG_VER,{NV_ENC_CONFIG_VER} };

	NVENCCALL(nvencAPI.nvEncGetEncodePresetConfigEx(encoder, codec, preset, tuningInfo, &presetConfig));

	NV_ENC_CONFIG config;
	memcpy(&config, &presetConfig.presetCfg, sizeof(NV_ENC_CONFIG));
	config.version = NV_ENC_CONFIG_VER;
	config.profileGUID = profile;

	//high quality encode
	config.gopLength = 120;
	config.frameIntervalP = 1;
	config.encodeCodecConfig.hevcConfig.idrPeriod = config.gopLength;

	config.rcParams.rateControlMode = NV_ENC_PARAMS_RC_VBR;
	config.rcParams.averageBitRate = 12000000U;
	config.rcParams.maxBitRate = 14000000U;
	config.rcParams.multiPass = NV_ENC_TWO_PASS_FULL_RESOLUTION;
	config.rcParams.vbvBufferSize = config.rcParams.maxBitRate * 4;
	config.rcParams.vbvInitialDelay = 0;
	config.rcParams.enableAQ = 1;

	NV_ENC_INITIALIZE_PARAMS encoderParams = { NV_ENC_INITIALIZE_PARAMS_VER };
	encoderParams.bufferFormat = bufferFormat;
	encoderParams.encodeConfig = &config;
	encoderParams.encodeGUID = codec;
	encoderParams.presetGUID = preset;
	encoderParams.tuningInfo = tuningInfo;
	encoderParams.encodeWidth = Graphics::getWidth();
	encoderParams.encodeHeight = Graphics::getHeight();
	encoderParams.darWidth = Graphics::getWidth();
	encoderParams.darHeight = Graphics::getHeight();
	encoderParams.maxEncodeWidth = Graphics::getWidth();
	encoderParams.maxEncodeHeight = Graphics::getHeight();
	encoderParams.frameRateNum = frameRate;
	encoderParams.frameRateDen = 1;
	encoderParams.enablePTD = 1;
	encoderParams.enableOutputInVidmem = 0;
	encoderParams.enableEncodeAsync = 0;

	NVENCCALL(nvencAPI.nvEncInitializeEncoder(encoder, &encoderParams));

	std::cout << "[class NvidiaEncoder] render at " << Graphics::getWidth() << " x " << Graphics::getHeight() << "\n";
	std::cout << "[class NvidiaEncoder] frameRate " << frameRate << "\n";
	std::cout << "[class NvidiaEncoder] frameToEncode " << frameToEncode << "\n";
	std::cout << "[class NvidiaEncoder] start encoding\n";

	avformat_network_init();

	avformat_alloc_output_context2(&outCtx, nullptr, "mp4", nullptr);

	outCtx->url = av_strdup("output.mp4");

	outStream = avformat_new_stream(outCtx, nullptr);

	outStream->id = 0;

	AVCodecParameters* vpar = outStream->codecpar;
	vpar->codec_id = AV_CODEC_ID_HEVC;
	vpar->codec_type = AVMEDIA_TYPE_VIDEO;
	vpar->width = Graphics::getWidth();
	vpar->height = Graphics::getHeight();

	avio_open(&outCtx->pb, outCtx->url, AVIO_FLAG_WRITE);

	avformat_write_header(outCtx, nullptr);

	pkt = av_packet_alloc();
}

NvidiaEncoder::~NvidiaEncoder()
{
	if (moduleNvEncAPI)
	{
		NVENCCALL(nvencAPI.nvEncDestroyEncoder(encoder));
		delete readbackHeap;
		FreeLibrary(moduleNvEncAPI);

		av_packet_free(&pkt);
		av_write_trailer(outCtx);
		avio_close(outCtx->pb);
		avformat_free_context(outCtx);
	}
}

bool NvidiaEncoder::encode(ID3D12Resource* const inputTexture)
{
	timeStart = timer.now();

	NV_ENC_REGISTER_RESOURCE registerInputResource = { NV_ENC_REGISTER_RESOURCE_VER };
	registerInputResource.bufferFormat = bufferFormat;
	registerInputResource.bufferUsage = NV_ENC_INPUT_IMAGE;
	registerInputResource.resourceType = NV_ENC_INPUT_RESOURCE_TYPE_DIRECTX;
	registerInputResource.resourceToRegister = inputTexture;
	registerInputResource.subResourceIndex = 0;
	registerInputResource.width = Graphics::getWidth();
	registerInputResource.height = Graphics::getHeight();
	registerInputResource.pitch = 0;
	registerInputResource.pInputFencePoint = nullptr;

	NVENCCALL(nvencAPI.nvEncRegisterResource(encoder, &registerInputResource));

	NV_ENC_REGISTER_RESOURCE registerOutputResource = { NV_ENC_REGISTER_RESOURCE_VER };
	registerOutputResource.bufferFormat = NV_ENC_BUFFER_FORMAT_U8;
	registerOutputResource.bufferUsage = NV_ENC_OUTPUT_BITSTREAM;
	registerOutputResource.resourceType = NV_ENC_INPUT_RESOURCE_TYPE_DIRECTX;
	registerOutputResource.resourceToRegister = readbackHeap->getResource();
	registerOutputResource.subResourceIndex = 0;
	registerOutputResource.width = 2 * 4 * Graphics::getWidth() * Graphics::getHeight();
	registerOutputResource.height = 1;
	registerOutputResource.pitch = 0;
	registerOutputResource.pInputFencePoint = nullptr;

	NVENCCALL(nvencAPI.nvEncRegisterResource(encoder, &registerOutputResource));

	NV_ENC_MAP_INPUT_RESOURCE mapInputResource = { NV_ENC_MAP_INPUT_RESOURCE_VER };
	mapInputResource.registeredResource = registerInputResource.registeredResource;

	NVENCCALL(nvencAPI.nvEncMapInputResource(encoder, &mapInputResource));

	NV_ENC_MAP_INPUT_RESOURCE mapOutputResource = { NV_ENC_MAP_INPUT_RESOURCE_VER };
	mapOutputResource.registeredResource = registerOutputResource.registeredResource;

	NVENCCALL(nvencAPI.nvEncMapInputResource(encoder, &mapOutputResource));

	NV_ENC_INPUT_RESOURCE_D3D12 inputResource = { NV_ENC_INPUT_RESOURCE_D3D12_VER };
	inputResource.pInputBuffer = mapInputResource.mappedResource;
	inputResource.inputFencePoint = NV_ENC_FENCE_POINT_D3D12{ NV_ENC_FENCE_POINT_D3D12_VER };

	InterlockedIncrement(&outputFenceValue);

	NV_ENC_OUTPUT_RESOURCE_D3D12 outputResource = { NV_ENC_INPUT_RESOURCE_D3D12_VER };
	outputResource.pOutputBuffer = mapOutputResource.mappedResource;
	outputResource.outputFencePoint = NV_ENC_FENCE_POINT_D3D12{ NV_ENC_FENCE_POINT_D3D12_VER };
	outputResource.outputFencePoint.pFence = outputFence.Get();
	outputResource.outputFencePoint.signalValue = outputFenceValue;
	outputResource.outputFencePoint.bSignal = true;

	NV_ENC_PIC_PARAMS picParams = { NV_ENC_PIC_PARAMS_VER };

	if (frameEncoded == frameToEncode)
	{
		picParams.encodePicFlags = NV_ENC_PIC_FLAG_EOS;

		NVENCCALL(nvencAPI.nvEncEncodePicture(encoder, &picParams));

		encoding = false;

		std::cout << "\n[class NvidiaEncoder] encode complete!\n";

		std::cout << "[class NvidiaEncoder] frame encode avg speed " << frameToEncode / encodeTime << "\n";
	}
	else
	{
		picParams.pictureStruct = NV_ENC_PIC_STRUCT_FRAME;

		picParams.inputBuffer = &inputResource;

		picParams.outputBitstream = &outputResource;

		picParams.bufferFmt = bufferFormat;

		picParams.inputWidth = Graphics::getWidth();

		picParams.inputHeight = Graphics::getHeight();

		picParams.completionEvent = nullptr;

		NVENCCALL(nvencAPI.nvEncEncodePicture(encoder, &picParams));

		NV_ENC_LOCK_BITSTREAM lockBitstream = { NV_ENC_LOCK_BITSTREAM_VER };

		lockBitstream.outputBitstream = &outputResource;

		lockBitstream.doNotWait = 0;

		NVENCCALL(nvencAPI.nvEncLockBitstream(encoder, &lockBitstream));

		pkt->pts = av_rescale_q(frameEncoded, AVRational{ 1,(int)frameRate }, outStream->time_base);
		pkt->dts = pkt->pts;
		pkt->stream_index = outStream->index;
		pkt->data = (uint8_t*)lockBitstream.bitstreamBufferPtr;
		pkt->size = lockBitstream.bitstreamSizeInBytes;

		if (!memcmp(lockBitstream.bitstreamBufferPtr, "\x00\x00\x00\x01\x67", 5))
		{
			pkt->flags |= AV_PKT_FLAG_KEY;
		}

		av_write_frame(outCtx, pkt);

		av_write_frame(outCtx, nullptr);

		NVENCCALL(nvencAPI.nvEncUnlockBitstream(encoder, lockBitstream.outputBitstream));
	}

	NVENCCALL(nvencAPI.nvEncUnmapInputResource(encoder, mapInputResource.mappedResource));

	NVENCCALL(nvencAPI.nvEncUnregisterResource(encoder, registerInputResource.registeredResource));

	NVENCCALL(nvencAPI.nvEncUnmapInputResource(encoder, mapOutputResource.mappedResource));

	NVENCCALL(nvencAPI.nvEncUnregisterResource(encoder, registerOutputResource.registeredResource));

	frameEncoded++;

	displayProgress();

	timeEnd = timer.now();

	const float frameTime = std::chrono::duration_cast<std::chrono::milliseconds>(timeEnd - timeStart).count() / 1000.f;

	encodeTime += frameTime;

	return encoding;
}
