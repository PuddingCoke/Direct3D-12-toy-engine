#pragma once

#ifndef _FXAAEFFECT_H_
#define _FXAAEFFECT_H_

#include"Effect.h"

#include<Gear/CompiledShaders/ColorToColorLuma.h>
#include<Gear/CompiledShaders/FXAA.h>

class FXAAEffect :public Effect
{
public:

	FXAAEffect(GraphicsContext* const context, const uint32_t width, const uint32_t height);

	~FXAAEffect();

	TextureRenderView* process(TextureRenderView* const inputTexture) const;

	void imGUICall() override;

	void setFXAAQualitySubpix(const float fxaaQualitySubpix);

	void setFXAAQualityEdgeThreshold(const float fxaaQualityEdgeThreshold);

	void setFXAAQualityEdgeThresholdMin(const float fxaaQualityEdgeThresholdMin);

private:

	TextureRenderView* colorLumaTexture;

	struct FXAAParam
	{
		float fxaaQualityRcpFrame; // unused
		float fxaaQualitySubpix;
		float fxaaQualityEdgeThreshold;
		float fxaaQualityEdgeThresholdMin;
	} fxaaParam;

	Shader* colorToColorLumaPS;

	ComPtr<ID3D12PipelineState> colorToColorLumaState;

	Shader* fxaaPS;

	ComPtr<ID3D12PipelineState> fxaaState;

};

#endif // !_FXAAEFFECT_H_