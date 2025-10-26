#pragma once

#ifndef _BLOOMEFFECT_H_
#define _BLOOMEFFECT_H_

#include<Gear/Core/Resource/SwapTexture.h>

#include<Gear/Utils/Math.h>

#include"Effect.h"

class BloomEffect :public Effect
{
public:

	static constexpr uint32_t blurSteps = 5;

	static constexpr uint32_t iteration[blurSteps] = { 2,3,4,5,6 };

	static constexpr DirectX::XMUINT2 workGroupSize = { 60,16 };

	BloomEffect(GraphicsContext* const context, const uint32_t width, const uint32_t height, ResourceManager* const resManager);

	~BloomEffect();

	TextureRenderView* process(TextureRenderView* const inputTexture) const;

	void imGUICall() override;

	void setExposure(const float exposure);

	void setGamma(const float gamma);

	void setThreshold(const float threshold);

	void setIntensity(const float intensity);

	void setSoftThreshold(const float softThreshold);

	float getExposure() const;

	float getGamma() const;

private:

	void updateCurve(GraphicsContext* const context, const uint32_t index);

	Core::Shader* bloomFilter;

	ComPtr<ID3D12PipelineState> bloomFilterState;

	Core::Shader* bloomHBlur;

	ComPtr<ID3D12PipelineState> bloomHBlurState;

	Core::Shader* bloomVBlur;

	ComPtr<ID3D12PipelineState> bloomVBlurState;

	Core::Shader* bloomFinal;

	ComPtr<ID3D12PipelineState> bloomFinalState;

	Core::Shader* bloomDownSample;

	ComPtr<ID3D12PipelineState> bloomDownSampleState;

	Core::Shader* bloomKarisAverage;

	ComPtr<ID3D12PipelineState> bloomKarisAverageState;

	ComPtr<ID3D12PipelineState> bloomUpSampleState;

	DirectX::XMUINT2 resolutions[blurSteps];

	TextureRenderView* lensDirtTexture;

	SwapTexture* swapTexture[blurSteps];

	TextureRenderView* filteredTexture;

	StaticCBuffer* blurParamBuffer[blurSteps];

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

#endif // !_BLOOMEFFECT_H_