#pragma once

#ifndef _BLOOMEFFECT_H_
#define _BLOOMEFFECT_H_

#include"Effect.h"

#include<ImGUI/imgui.h>

#include<Gear/CompiledShaders/BloomFilterPS.h>
#include<Gear/CompiledShaders/BloomFinalPS.h>
#include<Gear/CompiledShaders/BloomVBlurCS.h>
#include<Gear/CompiledShaders/BloomHBlurCS.h>
#include<Gear/CompiledShaders/BloomDownSamplePS.h>
#include<Gear/CompiledShaders/BloomKarisAveragePS.h>

#include<Gear/Core/Resource/SwapTexture.h>

#include<Gear/Core/Shader.h>

#include<Gear/Core/States.h>

#include<Gear/Utils/Math.h>

class BloomEffect :public Effect
{
public:

	static constexpr UINT blurSteps = 5;

	static constexpr unsigned int iteration[blurSteps] = { 2,3,4,5,6 };

	static constexpr DirectX::XMUINT2 workGroupSize = { 60,16 };

	BloomEffect(GraphicsContext* const context, const UINT width, const UINT height, ResourceManager* const resManager);

	~BloomEffect();

	TextureRenderTarget* process(TextureRenderTarget* const inputTexture) const;

	void imGuiCommand();

	void setExposure(const float exposure);

	void setGamma(const float gamma);

	void setThreshold(const float threshold);

	void setIntensity(const float intensity);

	void setSoftThreshold(const float softThreshold);

private:

	void updateCurve(const UINT index);

	Shader* bloomFilter;

	ComPtr<ID3D12PipelineState> bloomFilterState;

	Shader* bloomHBlur;

	ComPtr<ID3D12PipelineState> bloomHBlurState;

	Shader* bloomVBlur;

	ComPtr<ID3D12PipelineState> bloomVBlurState;

	Shader* bloomFinal;

	ComPtr<ID3D12PipelineState> bloomFinalState;

	Shader* bloomDownSample;

	ComPtr<ID3D12PipelineState> bloomDownSampleState;

	Shader* bloomKarisAverage;

	ComPtr<ID3D12PipelineState> bloomKarisAverageState;

	ComPtr<ID3D12PipelineState> bloomUpSampleState;

	DirectX::XMUINT2 resolutions[blurSteps];

	TextureRenderTarget* lensDirtTexture;

	SwapTexture* swapTexture[blurSteps];

	TextureRenderTarget* filteredTexture;

	ConstantBuffer* blurParamBuffer[blurSteps];

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
		UINT iteration;
		float sigma;
		DirectX::XMFLOAT4 padding[11];
	}blurParam[blurSteps];

};

#endif // !_BLOOMEFFECT_H_