#pragma once

#ifndef _NVIDIAENCODER_H_
#define _NVIDIAENCODER_H_

#include"Encoder.h"

#include<Gear/Core/DX/Resource/ReadbackHeap.h>

#include<NvEnc/nvEncodeAPI.h>

extern "C"
{
#include<libavutil/avutil.h>
#include<libavformat/avformat.h>
#include<libavcodec/avcodec.h>
}

#define NVENCCALL(func) \
{\
NVENCSTATUS status=func;\
if(status!=NV_ENC_SUCCESS)\
{\
std::cout<<"error occured at function "<<#func<<"\n";\
const char* error = nvencAPI.nvEncGetLastErrorString(encoder);\
std::cout << error << "\n";\
__debugbreak();\
}\
}\

//basic work flow

//Preperation
//1.open encode session
//2.initialize encoder

//Encoding
//1.register input resource
//2.map input resource
//3.register output resource
//4.map output resource
//5.encode picture

class NvidiaEncoder :public Encoder
{
public:

	NvidiaEncoder() = delete;

	NvidiaEncoder(const NvidiaEncoder&) = delete;

	NvidiaEncoder(const UINT frameToEncode);

	~NvidiaEncoder();

	bool encode(Texture* const inputTexture) override;

private:

	static constexpr NV_ENC_BUFFER_FORMAT bufferFormat = NV_ENC_BUFFER_FORMAT_ARGB;

	static constexpr NV_ENC_TUNING_INFO tuningInfo = NV_ENC_TUNING_INFO_HIGH_QUALITY;

	const GUID codec = NV_ENC_CODEC_HEVC_GUID;

	const GUID preset = NV_ENC_PRESET_P7_GUID;

	const GUID profile = NV_ENC_HEVC_PROFILE_MAIN_GUID;

	HMODULE moduleNvEncAPI;

	NV_ENCODE_API_FUNCTION_LIST nvencAPI;

	void* encoder;

	ReadbackHeap* readbackHeap;

	ComPtr<ID3D12Fence> outputFence;

	UINT outputFenceValue;

	AVFormatContext* outCtx;

	AVStream* outStream;

	AVPacket* pkt;

};

#endif // !_NVIDIAENCODER_H_
