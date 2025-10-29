﻿#include<Gear/Core/Effect/BloomEffect.h>

#include<Gear/Core/GlobalShader.h>

#include<Gear/Utils/File.h>

#include<Gear/CompiledShaders/BloomFilterPS.h>

#include<Gear/CompiledShaders/BloomFinalPS.h>

#include<Gear/CompiledShaders/BloomVBlurCS.h>

#include<Gear/CompiledShaders/BloomHBlurCS.h>

#include<Gear/CompiledShaders/BloomDownSamplePS.h>

#include<Gear/CompiledShaders/BloomKarisAveragePS.h>

Gear::Core::Effect::BloomEffect::BloomEffect(GraphicsContext* const context, const uint32_t width, const uint32_t height, ResourceManager* const resManager) :
	EffectBase(context, width, height, DXGI_FORMAT_R16G16B16A16_FLOAT),
	lensDirtTexture(resManager->createTextureRenderView(Gear::Utils::File::getRootFolder() + L"bloom_dirt_mask.png", true)),
	filteredTexture(ResourceManager::createTextureRenderView(width, height, DXGI_FORMAT_R16G16B16A16_FLOAT, 1, 1, false, true,
		DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_R16G16B16A16_FLOAT))
{
	filteredTexture->getTexture()->setName(L"Bloom Effect Filtered Texture");

	bloomFilter = new D3D12Core::Shader(g_BloomFilterPSBytes, sizeof(g_BloomFilterPSBytes));
	bloomFinal = new D3D12Core::Shader(g_BloomFinalPSBytes, sizeof(g_BloomFinalPSBytes));
	bloomVBlur = new D3D12Core::Shader(g_BloomVBlurCSBytes, sizeof(g_BloomVBlurCSBytes));
	bloomHBlur = new D3D12Core::Shader(g_BloomHBlurCSBytes, sizeof(g_BloomHBlurCSBytes));
	bloomDownSample = new D3D12Core::Shader(g_BloomDownSamplePSBytes, sizeof(g_BloomDownSamplePSBytes));
	bloomKarisAverage = new D3D12Core::Shader(g_BloomKarisAveragePSBytes, sizeof(g_BloomKarisAveragePSBytes));

	{
		const float sigma[blurSteps] = { 0.44f,0.57f,0.8f,1.32f,3.3f };

		for (uint32_t i = 0; i < blurSteps; i++)
		{
			resolutions[i] = DirectX::XMUINT2(width >> (i + 1), height >> (i + 1));

			swapTexture[i] = new Resource::SwapTexture(
				[=] {
					return ResourceManager::createTextureRenderView(resolutions[i].x, resolutions[i].y, DXGI_FORMAT_R16G16B16A16_FLOAT, 1, 1, false, true,
						DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_R16G16B16A16_FLOAT);
				}
			);

			blurParam[i].texelSize = DirectX::XMFLOAT2(1.f / resolutions[i].x, 1.f / resolutions[i].y);
			blurParam[i].iteration = iteration[i];
			blurParam[i].sigma = sigma[i];

			blurParamBuffer[i] = ResourceManager::createStaticCBuffer(sizeof(BlurParam), true);

			const std::wstring name = L"Blur Param Buffer ";

			blurParamBuffer[i]->getBuffer()->setName((name + std::to_wstring(i)).c_str());

			updateCurve(i);
		}
	}

	bloomParam.exposure = 1.f;
	bloomParam.gamma = 1.f;
	bloomParam.threshold = 1.f;
	bloomParam.intensity = 1.f;
	bloomParam.softThreshold = 1.f;

	//PS BlendState
	auto getDefaultPipelineState =
		[] {
		D3D12_GRAPHICS_PIPELINE_STATE_DESC desc = PipelineStateHelper::getDefaultGraphicsDesc();
		desc.InputLayout = {};
		desc.VS = GlobalShader::getFullScreenVS()->getByteCode();
		desc.RasterizerState = PipelineStateHelper::rasterCullBack;
		desc.DepthStencilState.DepthEnable = FALSE;
		desc.DepthStencilState.StencilEnable = FALSE;
		desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		desc.NumRenderTargets = 1;
		desc.RTVFormats[0] = DXGI_FORMAT_R16G16B16A16_FLOAT;
		return desc;
		};

	{
		D3D12_GRAPHICS_PIPELINE_STATE_DESC desc = getDefaultPipelineState();
		desc.PS = bloomFilter->getByteCode();
		desc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);

		GraphicsDevice::get()->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(&bloomFilterState));
	}

	{
		D3D12_GRAPHICS_PIPELINE_STATE_DESC desc = getDefaultPipelineState();
		desc.PS = bloomKarisAverage->getByteCode();
		desc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);

		GraphicsDevice::get()->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(&bloomKarisAverageState));
	}

	{
		D3D12_GRAPHICS_PIPELINE_STATE_DESC desc = getDefaultPipelineState();
		desc.PS = bloomDownSample->getByteCode();
		desc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);

		GraphicsDevice::get()->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(&bloomDownSampleState));
	}

	{
		D3D12_GRAPHICS_PIPELINE_STATE_DESC desc = getDefaultPipelineState();
		desc.PS = GlobalShader::getFullScreenPS()->getByteCode();
		desc.BlendState = PipelineStateHelper::blendAddtive;

		GraphicsDevice::get()->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(&bloomUpSampleState));
	}

	{
		D3D12_GRAPHICS_PIPELINE_STATE_DESC desc = getDefaultPipelineState();
		desc.PS = bloomFinal->getByteCode();
		desc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);

		GraphicsDevice::get()->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(&bloomFinalState));
	}

	PipelineStateHelper::createComputeState(&bloomHBlurState, bloomHBlur);

	PipelineStateHelper::createComputeState(&bloomVBlurState, bloomVBlur);
}

Gear::Core::Effect::BloomEffect::~BloomEffect()
{
	for (uint32_t i = 0; i < blurSteps; i++)
	{
		delete swapTexture[i];
		delete blurParamBuffer[i];
	}

	delete filteredTexture;

	delete bloomFilter;
	delete bloomFinal;

	delete lensDirtTexture;

	delete bloomHBlur;
	delete bloomVBlur;
	delete bloomDownSample;
	delete bloomKarisAverage;
}

Gear::Core::Resource::TextureRenderView* Gear::Core::Effect::BloomEffect::process(Resource::TextureRenderView* const inputTexture) const
{
	context->setTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	context->setViewportSimple(width, height);
	context->setPipelineState(bloomFilterState.Get());
	context->setRenderTargets({ filteredTexture->getRTVMipHandle(0) }, {});
	context->setPSConstants({ inputTexture->getAllSRVIndex() }, 0);
	context->setPSConstants(5, &bloomParam, 1);
	context->transitionResources();
	context->draw(3, 1, 0, 0);

	context->setViewportSimple(resolutions[0].x, resolutions[0].y);
	context->setPipelineState(bloomKarisAverageState.Get());
	context->setRenderTargets({ swapTexture[0]->write()->getRTVMipHandle(0) }, {});
	context->setPSConstants({ filteredTexture->getAllSRVIndex() }, 0);
	context->setPSConstants(4, &bloomParam, 1);
	context->transitionResources();
	context->draw(3, 1, 0, 0);
	swapTexture[0]->swap();

	context->setPipelineState(bloomDownSampleState.Get());

	for (uint32_t i = 0; i < blurSteps - 1; i++)
	{
		context->setViewportSimple(resolutions[i + 1].x, resolutions[i + 1].y);
		context->setRenderTargets({ swapTexture[i + 1]->write()->getRTVMipHandle(0) }, {});
		context->setPSConstants({ swapTexture[i]->read()->getAllSRVIndex() }, 0);
		context->transitionResources();
		context->draw(3, 1, 0, 0);
		swapTexture[i + 1]->swap();
	}

	context->setCSConstants({
		swapTexture[blurSteps - 1]->read()->getAllSRVIndex(),
		swapTexture[blurSteps - 1]->write()->getUAVMipIndex(0) }, 0);

	context->setCSConstantBuffer(blurParamBuffer[blurSteps - 1]);

	context->setPipelineState(bloomHBlurState.Get());
	context->transitionResources();
	context->dispatch(resolutions[blurSteps - 1].x / workGroupSize.x, resolutions[blurSteps - 1].y / workGroupSize.y + 1, 1);
	context->uavBarrier({ swapTexture[blurSteps - 1]->write()->getTexture() });
	swapTexture[blurSteps - 1]->swap();

	context->setCSConstants({
		swapTexture[blurSteps - 1]->read()->getAllSRVIndex(),
		swapTexture[blurSteps - 1]->write()->getUAVMipIndex(0) }, 0);

	context->setCSConstantBuffer(blurParamBuffer[blurSteps - 1]);

	context->setPipelineState(bloomVBlurState.Get());
	context->transitionResources();
	context->dispatch(resolutions[blurSteps - 1].x / workGroupSize.x, resolutions[blurSteps - 1].y / workGroupSize.y + 1, 1);
	context->uavBarrier({ swapTexture[blurSteps - 1]->write()->getTexture() });
	swapTexture[blurSteps - 1]->swap();

	for (uint32_t i = 0; i < blurSteps - 1; i++)
	{
		context->setCSConstantBuffer(blurParamBuffer[blurSteps - 2 - i]);

		context->setCSConstants({
			swapTexture[blurSteps - 2 - i]->read()->getAllSRVIndex(),
			swapTexture[blurSteps - 2 - i]->write()->getUAVMipIndex(0) }, 0);
		context->setPipelineState(bloomHBlurState.Get());
		context->transitionResources();
		context->dispatch(resolutions[blurSteps - 2 - i].x / workGroupSize.x, resolutions[blurSteps - 2 - i].y / workGroupSize.y + 1, 1);
		context->uavBarrier({ swapTexture[blurSteps - 2 - i]->write()->getTexture() });
		swapTexture[blurSteps - 2 - i]->swap();

		context->setCSConstants({
			swapTexture[blurSteps - 2 - i]->read()->getAllSRVIndex(),
			swapTexture[blurSteps - 2 - i]->write()->getUAVMipIndex(0) }, 0);
		context->setPipelineState(bloomVBlurState.Get());
		context->transitionResources();
		context->dispatch(resolutions[blurSteps - 2 - i].x / workGroupSize.x, resolutions[blurSteps - 2 - i].y / workGroupSize.y + 1, 1);
		context->uavBarrier({ swapTexture[blurSteps - 2 - i]->write()->getTexture() });

		context->setViewportSimple(resolutions[blurSteps - 2 - i].x, resolutions[blurSteps - 2 - i].y);
		context->setPipelineState(bloomUpSampleState.Get());
		context->setRenderTargets({ swapTexture[blurSteps - 2 - i]->write()->getRTVMipHandle(0) }, {});
		context->setPSConstants({ swapTexture[blurSteps - 1 - i]->read()->getAllSRVIndex() }, 0);
		context->transitionResources();
		context->draw(3, 1, 0, 0);
		swapTexture[blurSteps - 2 - i]->swap();
	}

	context->setViewportSimple(width, height);
	context->setPipelineState(bloomFinalState.Get());
	context->setRenderTargets({ outputTexture->getRTVMipHandle(0) }, {});
	context->setPSConstants({
		inputTexture->getAllSRVIndex(),
		swapTexture[0]->read()->getAllSRVIndex(),
		lensDirtTexture->getAllSRVIndex() }, 0);
	context->setPSConstants(4, &bloomParam, 3);
	context->transitionResources();
	context->draw(3, 1, 0, 0);

	return outputTexture;
}

void Gear::Core::Effect::BloomEffect::imGUICall()
{
	ImGui::Begin("Bloom Effect");
	ImGui::SliderFloat("Exposure", &bloomParam.exposure, 0.0f, 4.f);
	ImGui::SliderFloat("Gamma", &bloomParam.gamma, 0.0f, 4.f);
	ImGui::SliderFloat("Threshold", &bloomParam.threshold, 0.0f, 1.f);
	ImGui::SliderFloat("SoftThreshold", &bloomParam.softThreshold, 0.0f, 1.f);
	ImGui::SliderFloat("Intensity", &bloomParam.intensity, 0.0f, 4.f);
	ImGui::End();
}

void Gear::Core::Effect::BloomEffect::setExposure(const float exposure)
{
	bloomParam.exposure = exposure;
}

void Gear::Core::Effect::BloomEffect::setGamma(const float gamma)
{
	bloomParam.gamma = gamma;
}

void Gear::Core::Effect::BloomEffect::setThreshold(const float threshold)
{
	bloomParam.threshold = threshold;
}

void Gear::Core::Effect::BloomEffect::setIntensity(const float intensity)
{
	bloomParam.intensity = intensity;
}

void Gear::Core::Effect::BloomEffect::setSoftThreshold(const float softThreshold)
{
	bloomParam.softThreshold = softThreshold;
}

float Gear::Core::Effect::BloomEffect::getExposure() const
{
	return bloomParam.exposure;
}

float Gear::Core::Effect::BloomEffect::getGamma() const
{
	return bloomParam.gamma;
}

void Gear::Core::Effect::BloomEffect::updateCurve(const uint32_t index)
{
	blurParam[index].weight[0] = Gear::Utils::Math::gauss(blurParam[index].sigma, 0.f);

	for (uint32_t i = 1; i < (iteration[index] - 1) * 2 + 1; i += 2)
	{
		const float g1 = Gear::Utils::Math::gauss(blurParam[index].sigma, (float)i);
		const float g2 = Gear::Utils::Math::gauss(blurParam[index].sigma, (float)(i + 1));
		blurParam[index].weight[(i + 1) / 2] = g1 + g2;
		blurParam[index].offset[(i + 1) / 2] = (g1 * i + g2 * (i + 1)) / (g1 + g2);
	}

	context->updateBuffer(blurParamBuffer[index], &blurParam[index], sizeof(BlurParam));
}
