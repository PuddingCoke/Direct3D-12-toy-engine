#pragma once

#include<Gear/Core/RenderEngine.h>

class WaveCascade
{
public:

	WaveCascade(const UINT size, GraphicsContext* const context) :
		size(size), context(context)
	{
		waveDataTexture = createTexture(size, DXGI_FORMAT_R32G32B32A32_FLOAT);

		waveSpectrumTexture = createTexture(size, DXGI_FORMAT_R32G32B32A32_FLOAT);

		DxDz = createTexture(size, DXGI_FORMAT_R32G32_FLOAT);

		DyDxz = createTexture(size, DXGI_FORMAT_R32G32_FLOAT);

		DyxDyz = createTexture(size, DXGI_FORMAT_R32G32_FLOAT);

		DxxDzz = createTexture(size, DXGI_FORMAT_R32G32_FLOAT);

		displacementTexture = createTexture(size, DXGI_FORMAT_R32G32B32A32_FLOAT);

		derivativeTexture = createTexture(size, DXGI_FORMAT_R32G32B32A32_FLOAT);

		jacobianTexture = createTexture(size, DXGI_FORMAT_R32_FLOAT);

		displacementTexture->getTexture()->setName(L"displacementTexture");

		derivativeTexture->getTexture()->setName(L"derivativeTexture");

		jacobianTexture->getTexture()->setName(L"jacobianTexture");
	}

	~WaveCascade()
	{
		delete waveDataTexture;

		delete waveSpectrumTexture;

		delete DxDz;

		delete DyDxz;

		delete DyxDyz;

		delete DxxDzz;

		delete displacementTexture;

		delete derivativeTexture;

		delete jacobianTexture;
	}

	void ifftPermutation(TextureRenderView* const inputTexture)
	{
		context->setPipelineState(ifftState);

		context->setCSConstants({
			tempTexture->getUAVMipIndex(0),
			inputTexture->getAllSRVIndex() }, 0);

		context->transitionResources();

		context->dispatch(size, 1, 1);

		context->uavBarrier({
			tempTexture->getTexture() });

		context->setCSConstants({
			inputTexture->getUAVMipIndex(0),
			tempTexture->getAllSRVIndex() }, 0);

		context->transitionResources();

		context->dispatch(size, 1, 1);

		context->uavBarrier({
			inputTexture->getTexture() });

		context->setPipelineState(permutationState);

		context->setCSConstants({
			inputTexture->getUAVMipIndex(0) }, 0);

		context->transitionResources();

		context->dispatch(size / 8, size / 8, 1);

		context->uavBarrier({
			inputTexture->getTexture() });
	}

	void calculateInitialSpectrum(ImmutableCBuffer* const spectrumParamBuffer)
	{
		context->setPipelineState(spectrumState);

		context->setCSConstants({
			tildeh0Texture->getUAVMipIndex(0),
			waveDataTexture->getUAVMipIndex(0),
			randomGaussTexture->getAllSRVIndex() }, 0);

		context->setCSConstantBuffer(spectrumParamBuffer);

		context->transitionResources();

		context->dispatch(size / 8, size / 8, 1);

		context->uavBarrier({
			tildeh0Texture->getTexture(),
			waveDataTexture->getTexture() });

		context->setPipelineState(conjugateState);

		context->setCSConstants({
			waveSpectrumTexture->getUAVMipIndex(0),
			tildeh0Texture->getAllSRVIndex() }, 0);

		context->transitionResources();

		context->dispatch(size / 8, size / 8, 1);

		context->uavBarrier({
			waveSpectrumTexture->getTexture() });
	}

	void calculateTimeDependentSpectrum()
	{
		context->setPipelineState(displacementSpectrumState);

		context->setCSConstants({
			DxDz->getUAVMipIndex(0),
			DyDxz->getUAVMipIndex(0),
			DyxDyz->getUAVMipIndex(0),
			DxxDzz->getUAVMipIndex(0),
			waveDataTexture->getAllSRVIndex(),
			waveSpectrumTexture->getAllSRVIndex() }, 0);

		context->transitionResources();

		context->dispatch(size / 8, size / 8, 1);

		context->uavBarrier({
			DxDz->getTexture(),
			DyDxz->getTexture(),
			DyxDyz->getTexture(),
			DxxDzz->getTexture() });
	}

	void calculateDisplacementAndDerivative()
	{
		ifftPermutation(DxDz);

		ifftPermutation(DyDxz);

		ifftPermutation(DyxDyz);

		ifftPermutation(DxxDzz);

		context->setPipelineState(waveMergeState);

		context->setCSConstants({
			displacementTexture->getUAVMipIndex(0),
			derivativeTexture->getUAVMipIndex(0),
			jacobianTexture->getUAVMipIndex(0),
			DxDz->getAllSRVIndex(),
			DyDxz->getAllSRVIndex(),
			DyxDyz->getAllSRVIndex(),
			DxxDzz->getAllSRVIndex() }, 0);

		context->transitionResources();

		context->dispatch(size / 8, size / 8, 1);

		context->uavBarrier({
			displacementTexture->getTexture(),
			derivativeTexture->getTexture(),
			jacobianTexture->getTexture() });
	}

	static ID3D12PipelineState* spectrumState;

	static ID3D12PipelineState* conjugateState;

	static ID3D12PipelineState* displacementSpectrumState;

	static ID3D12PipelineState* ifftState;

	static ID3D12PipelineState* permutationState;

	static ID3D12PipelineState* waveMergeState;

	static TextureRenderView* tempTexture;

	static TextureRenderView* randomGaussTexture;

	//(tildeh0(k))
	//x y
	static TextureRenderView* tildeh0Texture;

	//Dx Dy Dz
	TextureRenderView* displacementTexture;

	//dDy/dx dDy/dz dDx/dx dDz/dz
	TextureRenderView* derivativeTexture;

	//J
	TextureRenderView* jacobianTexture;

private:

	static TextureRenderView* createTexture(const UINT& resolution, const DXGI_FORMAT& format)
	{
		return ResourceManager::createTextureRenderView(resolution, resolution, format, 1, 1, false, true, format, format, DXGI_FORMAT_UNKNOWN);
	}

	const UINT size;

	GraphicsContext* const context;

	//k.x 1.0/length(K) k.z angularSpeed
	//x y z w
	TextureRenderView* waveDataTexture;

	//(tildeh0(k), conj(tildeh0(-k)))
	//x y z w
	TextureRenderView* waveSpectrumTexture;

	//Dx Dz
	TextureRenderView* DxDz;

	//Dy dDx/dz
	TextureRenderView* DyDxz;

	//dDy/dx dDy/dz
	TextureRenderView* DyxDyz;

	//dDx/dx dDz/dz
	TextureRenderView* DxxDzz;

};

ID3D12PipelineState* WaveCascade::spectrumState = nullptr;

ID3D12PipelineState* WaveCascade::conjugateState = nullptr;

ID3D12PipelineState* WaveCascade::displacementSpectrumState = nullptr;

ID3D12PipelineState* WaveCascade::ifftState = nullptr;

ID3D12PipelineState* WaveCascade::permutationState = nullptr;

ID3D12PipelineState* WaveCascade::waveMergeState = nullptr;

TextureRenderView* WaveCascade::tempTexture = nullptr;

TextureRenderView* WaveCascade::randomGaussTexture = nullptr;

TextureRenderView* WaveCascade::tildeh0Texture = nullptr;