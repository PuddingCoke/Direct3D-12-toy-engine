#pragma once

#ifndef _NVIDIAENCODER_H_
#define _NVIDIAENCODER_H_

#include<iomanip>

#include<Gear/Core/Graphics.h>

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

class NvidiaEncoder
{
public:

	NvidiaEncoder() = delete;

	NvidiaEncoder(const NvidiaEncoder&) = delete;

	NvidiaEncoder(const UINT frameToEncode);

	~NvidiaEncoder();

	bool encode(ID3D12Resource* const inputTexture);

private:

	static constexpr UINT frameRate = 60;

	static constexpr NV_ENC_BUFFER_FORMAT bufferFormat = NV_ENC_BUFFER_FORMAT_ARGB;

	HANDLE hConsole;

	COORD progressBarOutputLocation;

	UINT frameEncoded;

	const UINT frameToEncode;

	bool encoding;

	void* encoder;

	NV_ENCODE_API_FUNCTION_LIST nvencAPI;

	ReadbackHeap* readbackHeap;

	HMODULE moduleNvEncAPI;

	ComPtr<ID3D12Fence> outputFence;

	UINT outputFenceValue;

	const GUID codec = NV_ENC_CODEC_HEVC_GUID;

	const GUID preset = NV_ENC_PRESET_P7_GUID;

	const GUID profile = NV_ENC_HEVC_PROFILE_MAIN_GUID;

	const NV_ENC_TUNING_INFO tuningInfo = NV_ENC_TUNING_INFO_HIGH_QUALITY;

	std::chrono::steady_clock timer;

	std::chrono::steady_clock::time_point timeStart;

	std::chrono::steady_clock::time_point timeEnd;

	float encodeTime;

	AVFormatContext* outCtx;

	AVStream* outStream;

	AVPacket* pkt;

};

#endif // !_NVIDIAENCODER_H_
