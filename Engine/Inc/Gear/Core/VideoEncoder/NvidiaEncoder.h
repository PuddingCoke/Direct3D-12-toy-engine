#pragma once

#ifndef _CORE_VIDEOENCODER_NVIDIAENCODER_H_
#define _CORE_VIDEOENCODER_NVIDIAENCODER_H_

#include<Gear/Core/Resource/D3D12Resource/ReadbackHeap.h>

#include"Encoder.h"

#include<NvEnc/nvEncodeAPI.h>

#include<queue>

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
//6.lock bitstream
//7.get bitstream ptr
//8.unlock bitstream
//9.unmap input resource
//10.unregister input resource
//11.unmap output resource
//12.unregister output resource

namespace Core
{
	namespace VideoEncoder
	{
		class NvidiaEncoder :public Encoder
		{
		public:

			NvidiaEncoder() = delete;

			NvidiaEncoder(const NvidiaEncoder&) = delete;

			NvidiaEncoder(const uint32_t frameToEncode);

			~NvidiaEncoder();

			bool encode(Resource::D3D12Resource::Texture* const inputTexture) override;

			static constexpr uint32_t lookaheadDepth = 31;

		private:

			static constexpr NV_ENC_BUFFER_FORMAT bufferFormat = NV_ENC_BUFFER_FORMAT_ARGB;

			static constexpr NV_ENC_TUNING_INFO tuningInfo = NV_ENC_TUNING_INFO_HIGH_QUALITY;

			static constexpr OutputVideoFormat outputVideoFormat = OutputVideoFormat::H264;

			const GUID codec = NV_ENC_CODEC_H264_GUID;

			const GUID preset = NV_ENC_PRESET_P7_GUID;

			const GUID profile = NV_ENC_H264_PROFILE_HIGH_GUID;

			HMODULE moduleNvEncAPI;

			NV_ENCODE_API_FUNCTION_LIST nvencAPI;

			void* encoder;

			Resource::D3D12Resource::ReadbackHeap* readbackHeap;

			ComPtr<ID3D12Fence> outputFence;

			uint32_t outputFenceValue;

			std::queue<NV_ENC_REGISTERED_PTR> registeredInputResourcePtrs;

			std::queue<NV_ENC_INPUT_PTR> mappedInputResourcePtrs;

			std::queue<NV_ENC_OUTPUT_RESOURCE_D3D12> outputResources;

			NV_ENC_REGISTERED_PTR registeredOutputResourcePtr;

			NV_ENC_INPUT_PTR mappedOutputResourcePtr;

		};
	}
}

#endif // !_CORE_VIDEOENCODER_NVIDIAENCODER_H_