#pragma once

#ifndef _GEAR_CORE_EFFECT_FXAAEFFECT_H_
#define _GEAR_CORE_EFFECT_FXAAEFFECT_H_

#include"EffectBase.h"

namespace Gear
{
	namespace Core
	{
		namespace Effect
		{
			class FXAAEffect :public EffectBase
			{
			public:

				FXAAEffect(GraphicsContext* const context, const uint32_t width, const uint32_t height);

				~FXAAEffect();

				Resource::TextureRenderView* process(Resource::TextureRenderView* const inputTexture) const;

				void imGUICall() override;

				void setFXAAQualitySubpix(const float fxaaQualitySubpix);

				void setFXAAQualityEdgeThreshold(const float fxaaQualityEdgeThreshold);

				void setFXAAQualityEdgeThresholdMin(const float fxaaQualityEdgeThresholdMin);

			private:

				Resource::TextureRenderView* colorLumaTexture;

				struct FXAAParam
				{
					float fxaaQualityRcpFrame; // unused
					float fxaaQualitySubpix;
					float fxaaQualityEdgeThreshold;
					float fxaaQualityEdgeThresholdMin;
				} fxaaParam;

				D3D12Core::Shader* colorToColorLumaPS;

				ComPtr<ID3D12PipelineState> colorToColorLumaState;

				D3D12Core::Shader* fxaaPS;

				ComPtr<ID3D12PipelineState> fxaaState;

			};
		}
	}
}

#endif // !_FXAAEFFECT_H_