#include<Gear/Core/VideoEncoder/NvidiaEncoder.h>

NvidiaEncoder::NvidiaEncoder(const uint32_t frameToEncode) :
	Encoder(frameToEncode), encoder(nullptr),
	readbackHeap(new ReadbackHeap(2 * 4 * Graphics::getWidth() * Graphics::getHeight())),
	outCtx(nullptr), outStream(nullptr), pkt(nullptr),
	nvencAPI{ NV_ENCODE_API_FUNCTION_LIST_VER },
	outputFenceValue(0)
{
	moduleNvEncAPI = LoadLibraryA("nvEncodeAPI64.dll");

	if (moduleNvEncAPI == 0)
	{
		throw "unable to load nvEncodeAPI64.dll";
	}

	NVENCSTATUS(__stdcall * NVENCAPICreateInstance)(NV_ENCODE_API_FUNCTION_LIST*) = (NVENCSTATUS(*)(NV_ENCODE_API_FUNCTION_LIST*))GetProcAddress(moduleNvEncAPI, "NvEncodeAPICreateInstance");

	LOGENGINE("api create instance status", static_cast<uint32_t>(NVENCAPICreateInstance(&nvencAPI)));

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
	config.rcParams.enableLookahead = 1;
	config.rcParams.lookaheadDepth = lookaheadDepth;
	config.rcParams.rateControlMode = NV_ENC_PARAMS_RC_VBR;
	config.rcParams.averageBitRate = 20000000U;
	config.rcParams.maxBitRate = 40000000U;
	config.rcParams.vbvBufferSize = config.rcParams.maxBitRate * 4;
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

	GraphicsDevice::get()->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&outputFence));

	LOGENGINE("frame rate", frameRate);

	LOGENGINE("frame to encode", frameToEncode);

	LOGENGINE("start encoding");

	avformat_alloc_output_context2(&outCtx, nullptr, "mp4", "output.mp4");

	outStream = avformat_new_stream(outCtx, nullptr);

	outStream->id = 0;

	AVCodecParameters* vpar = outStream->codecpar;
	vpar->codec_id = AV_CODEC_ID_H264;
	vpar->codec_type = AVMEDIA_TYPE_VIDEO;
	vpar->width = Graphics::getWidth();
	vpar->height = Graphics::getHeight();

	avio_open(&outCtx->pb, "output.mp4", AVIO_FLAG_WRITE);

	avformat_write_header(outCtx, nullptr);

	pkt = av_packet_alloc();

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

	registeredOutputResourcePtr = registerOutputResource.registeredResource;

	NV_ENC_MAP_INPUT_RESOURCE mapOutputResource = { NV_ENC_MAP_INPUT_RESOURCE_VER };
	mapOutputResource.registeredResource = registerOutputResource.registeredResource;

	NVENCCALL(nvencAPI.nvEncMapInputResource(encoder, &mapOutputResource));

	mappedOutputResourcePtr = mapOutputResource.mappedResource;
}

NvidiaEncoder::~NvidiaEncoder()
{
	if (moduleNvEncAPI)
	{
		nvencAPI.nvEncUnmapInputResource(encoder, mappedOutputResourcePtr);

		nvencAPI.nvEncUnregisterResource(encoder, registeredOutputResourcePtr);

		while (mappedInputResourcePtrs.size())
		{
			nvencAPI.nvEncUnmapInputResource(encoder, mappedInputResourcePtrs.front());

			mappedInputResourcePtrs.pop();
		}

		while (registeredInputResourcePtrs.size())
		{
			nvencAPI.nvEncUnregisterResource(encoder, registeredInputResourcePtrs.front());

			registeredInputResourcePtrs.pop();
		}

		NVENCCALL(nvencAPI.nvEncDestroyEncoder(encoder));

		delete readbackHeap;

		FreeLibrary(moduleNvEncAPI);

		av_packet_free(&pkt);
		av_write_trailer(outCtx);
		avio_close(outCtx->pb);
		avformat_free_context(outCtx);
	}
}

bool NvidiaEncoder::encode(Texture* const inputTexture)
{
	timeStart = std::chrono::steady_clock::now();

	if (frameEncoded == frameToEncode)
	{
		NV_ENC_PIC_PARAMS picParams = { NV_ENC_PIC_PARAMS_VER };

		picParams.encodePicFlags = NV_ENC_PIC_FLAG_EOS;

		NVENCCALL(nvencAPI.nvEncEncodePicture(encoder, &picParams));

		encoding = false;

		displayResult();
	}
	else
	{
		NV_ENC_REGISTER_RESOURCE registerInputResource = { NV_ENC_REGISTER_RESOURCE_VER };
		registerInputResource.bufferFormat = bufferFormat;
		registerInputResource.bufferUsage = NV_ENC_INPUT_IMAGE;
		registerInputResource.resourceType = NV_ENC_INPUT_RESOURCE_TYPE_DIRECTX;
		registerInputResource.resourceToRegister = inputTexture->getResource();
		registerInputResource.subResourceIndex = 0;
		registerInputResource.width = Graphics::getWidth();
		registerInputResource.height = Graphics::getHeight();
		registerInputResource.pitch = 0;
		registerInputResource.pInputFencePoint = nullptr;

		NVENCCALL(nvencAPI.nvEncRegisterResource(encoder, &registerInputResource));

		registeredInputResourcePtrs.push(registerInputResource.registeredResource);

		NV_ENC_MAP_INPUT_RESOURCE mapInputResource = { NV_ENC_MAP_INPUT_RESOURCE_VER };
		mapInputResource.registeredResource = registerInputResource.registeredResource;

		NVENCCALL(nvencAPI.nvEncMapInputResource(encoder, &mapInputResource));

		mappedInputResourcePtrs.push(mapInputResource.mappedResource);

		NV_ENC_INPUT_RESOURCE_D3D12 inputResource = { NV_ENC_INPUT_RESOURCE_D3D12_VER };
		inputResource.pInputBuffer = mapInputResource.mappedResource;
		inputResource.inputFencePoint = NV_ENC_FENCE_POINT_D3D12{ NV_ENC_FENCE_POINT_D3D12_VER };

		NV_ENC_OUTPUT_RESOURCE_D3D12 outputResource = { NV_ENC_INPUT_RESOURCE_D3D12_VER };
		outputResource.pOutputBuffer = mappedOutputResourcePtr;
		outputResource.outputFencePoint = NV_ENC_FENCE_POINT_D3D12{ NV_ENC_FENCE_POINT_D3D12_VER };
		outputResource.outputFencePoint.pFence = outputFence.Get();
		outputResource.outputFencePoint.signalValue = ++outputFenceValue;
		outputResource.outputFencePoint.bSignal = true;

		outputResources.push(outputResource);

		NV_ENC_PIC_PARAMS picParams = { NV_ENC_PIC_PARAMS_VER };

		picParams.pictureStruct = NV_ENC_PIC_STRUCT_FRAME;

		picParams.inputBuffer = &inputResource;

		picParams.outputBitstream = &outputResource;

		picParams.bufferFmt = bufferFormat;

		picParams.inputWidth = Graphics::getWidth();

		picParams.inputHeight = Graphics::getHeight();

		picParams.completionEvent = nullptr;

		const NVENCSTATUS status = nvencAPI.nvEncEncodePicture(encoder, &picParams);

		if ((outputResources.size() == lookaheadDepth + 1) && (status == NV_ENC_SUCCESS || status == NV_ENC_ERR_NEED_MORE_INPUT))
		{
			frameEncoded++;

			NV_ENC_LOCK_BITSTREAM lockBitstream = { NV_ENC_LOCK_BITSTREAM_VER };

			lockBitstream.outputBitstream = &outputResources.front();

			lockBitstream.doNotWait = 0;

			NVENCCALL(nvencAPI.nvEncLockBitstream(encoder, &lockBitstream));

			pkt->pts = av_rescale_q(frameEncoded, AVRational{ 1,static_cast<int>(frameRate) }, outStream->time_base);

			pkt->dts = pkt->pts;

			pkt->duration = av_rescale_q(1, AVRational{ 1,static_cast<int>(frameRate) }, outStream->time_base);

			pkt->stream_index = outStream->index;

			pkt->data = reinterpret_cast<uint8_t*>(lockBitstream.bitstreamBufferPtr);

			pkt->size = lockBitstream.bitstreamSizeInBytes;

			av_write_frame(outCtx, pkt);

			av_write_frame(outCtx, nullptr);

			NVENCCALL(nvencAPI.nvEncUnlockBitstream(encoder, lockBitstream.outputBitstream));

			outputResources.pop();

			nvencAPI.nvEncUnmapInputResource(encoder, mappedInputResourcePtrs.front());

			mappedInputResourcePtrs.pop();

			nvencAPI.nvEncUnregisterResource(encoder, registeredInputResourcePtrs.front());

			registeredInputResourcePtrs.pop();
		}
		else if (status != NV_ENC_SUCCESS && status != NV_ENC_ERR_NEED_MORE_INPUT)
		{
			const char* error = nvencAPI.nvEncGetLastErrorString(encoder);

			std::cout << "status " << status << "\n";

			std::cout << error << "\n";

			__debugbreak();
		}
	}

	timeEnd = std::chrono::steady_clock::now();

	const float frameTime = std::chrono::duration<float>(timeEnd - timeStart).count();

	encodeTime += frameTime;

	return encoding;
}
