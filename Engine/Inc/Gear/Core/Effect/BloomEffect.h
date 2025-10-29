#pragma once

#ifndef _CORE_EFFECT_BLOOMEFFECT_H_
#define _CORE_EFFECT_BLOOMEFFECT_H_

#include<Gear/Core/Resource/SwapTexture.h>

#include<Gear/Utils/Math.h>

#include"EffectBase.h"

namespace Core
{
	namespace Effect
	{
		class BloomEffect :public EffectBase
		{
		public:

			static constexpr uint32_t blurSteps = 5;

			static constexpr uint32_t iteration[blurSteps] = { 2,3,4,5,6 };

			static constexpr DirectX::XMUINT2 workGroupSize = { 60,16 };

			BloomEffect(GraphicsContext* const context, const uint32_t width, const uint32_t height, ResourceManager* const resManager);

			~BloomEffect();

			Resource::TextureRenderView* process(Resource::TextureRenderView* const inputTexture) const;

			void imGUICall() override;

			void setExposure(const float exposure);

			void setGamma(const float gamma);

			void setThreshold(const float threshold);

			void setIntensity(const float intensity);

			void setSoftThreshold(const float softThreshold);

			float getExposure() const;

			float getGamma() const;

		private:

			void updateCurve(const uint32_t index);

			D3D12Core::Shader* bloomFilter;

			ComPtr<ID3D12PipelineState> bloomFilterState;

			D3D12Core::Shader* bloomHBlur;

			ComPtr<ID3D12PipelineState> bloomHBlurState;

			D3D12Core::Shader* bloomVBlur;

			ComPtr<ID3D12PipelineState> bloomVBlurState;

			D3D12Core::Shader* bloomFinal;

			ComPtr<ID3D12PipelineState> bloomFinalState;

			D3D12Core::Shader* bloomDownSample;

			ComPtr<ID3D12PipelineState> bloomDownSampleState;

			D3D12Core::Shader* bloomKarisAverage;

			ComPtr<ID3D12PipelineState> bloomKarisAverageState;

			ComPtr<ID3D12PipelineState> bloomUpSampleState;

			DirectX::XMUINT2 resolutions[blurSteps];

			Resource::TextureRenderView* lensDirtTexture;

			Resource::SwapTexture* swapTexture[blurSteps];

			Resource::TextureRenderView* filteredTexture;

			Resource::StaticCBuffer* blurParamBuffer[blurSteps];

			struct BloomParam
			{
				float exposure;
				float gamma;
				float threshold;
				float intensity;
				float softThreshold;
			}bloomParam;

			struct BlurParam
			{
				float weight[8];
				float offset[8];
				DirectX::XMFLOAT2 texelSize;
				uint32_t iteration;
				float sigma;
				DirectX::XMFLOAT4 padding[11];
			}blurParam[blurSteps];

		};
	}
}

#endif // !_CORE_EFFECT_BLOOMEFFECT_H_