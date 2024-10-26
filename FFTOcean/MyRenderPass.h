#pragma once

#include<Gear/Core/RenderPass.h>

#include<Gear/Core/Shader.h>

#include<Gear/Core/Resource/TextureRenderView.h>

class MyRenderPass :public RenderPass
{
public:

	MyRenderPass() :
		spectrumParamBuffer(ResourceManager::createConstantBuffer(sizeof(SpectrumParam), true)),
		spectrumCS(new Shader(Utils::getRootFolder() + "SpectrumCS.cso")),
		conjugateCS(new Shader(Utils::getRootFolder() + "ConjugateCS.cso")),
		displacementSpectrumCS(new Shader(Utils::getRootFolder() + "DisplacementSpectrumCS.cso")),
		ifftCS(new Shader(Utils::getRootFolder() + "IFFTCS.cso")),
		permutationCS(new Shader(Utils::getRootFolder() + "PermutationCS.cso")),
		waveMergeCS(new Shader(Utils::getRootFolder() + "WaveMergeCS.cso"))
	{
		spectrumState = PipelineState::createComputeState(spectrumCS);

		conjugateState = PipelineState::createComputeState(conjugateCS);

		displacementSpectrumState = PipelineState::createComputeState(displacementSpectrumCS);

		ifftState = PipelineState::createComputeState(ifftCS);

		permutationState = PipelineState::createComputeState(permutationCS);

		waveMergeState = PipelineState::createComputeState(waveMergeCS);

		tildeh0Texture = createTexture(textureResolution + 1, DXGI_FORMAT_R32G32_FLOAT);

		waveDataTexture = createTexture(textureResolution + 1, DXGI_FORMAT_R32G32B32A32_FLOAT);

		waveSpectrumTexture = createTexture(textureResolution, DXGI_FORMAT_R32G32B32A32_FLOAT);

		tempTexture = createTexture(textureResolution, DXGI_FORMAT_R32G32_FLOAT);

		Dy = createTexture(textureResolution, DXGI_FORMAT_R32G32_FLOAT);

		Dx = createTexture(textureResolution, DXGI_FORMAT_R32G32_FLOAT);

		Dz = createTexture(textureResolution, DXGI_FORMAT_R32G32_FLOAT);

		Dyx = createTexture(textureResolution, DXGI_FORMAT_R32G32_FLOAT);

		Dyz = createTexture(textureResolution, DXGI_FORMAT_R32G32_FLOAT);

		Dxx = createTexture(textureResolution, DXGI_FORMAT_R32G32_FLOAT);

		Dzz = createTexture(textureResolution, DXGI_FORMAT_R32G32_FLOAT);

		Dxz = createTexture(textureResolution, DXGI_FORMAT_R32G32_FLOAT);

		displacementTexture = createTexture(textureResolution, DXGI_FORMAT_R32G32B32A32_FLOAT);

		derivativeTexture = createTexture(textureResolution, DXGI_FORMAT_R32G32B32A32_FLOAT);

		jacobianTexture = createTexture(textureResolution, DXGI_FORMAT_R32_FLOAT);

		begin();

		randomGaussTexture = resManager->createTextureRenderView(textureResolution + 1, textureResolution + 1, RandomDataType::GAUSS, true);

		calculateInitialSpectrum();

		{
			const float clearValue[] = { 999.f,999.f,999.f,999.f };

			context->clearUnorderedAccess(jacobianTexture->getClearUAVDesc(0), clearValue);
		}

		end();

		RenderEngine::get()->submitRenderPass(this);

		randomGaussTexture->getTexture()->setName(L"randomGaussTexture");

		tildeh0Texture->getTexture()->setName(L"tildeh0Texture");

		waveDataTexture->getTexture()->setName(L"waveDataTexture");

		waveSpectrumTexture->getTexture()->setName(L"waveSpectrumTexture");

		Dy->getTexture()->setName(L"Dy");

		Dx->getTexture()->setName(L"Dx");

		Dz->getTexture()->setName(L"Dz");

		Dyx->getTexture()->setName(L"Dyx");

		Dyz->getTexture()->setName(L"Dyz");

		Dxx->getTexture()->setName(L"Dxx");

		Dzz->getTexture()->setName(L"Dzz");

		Dxz->getTexture()->setName(L"Dxz");

		displacementTexture->getTexture()->setName(L"displacementTexture");

		derivativeTexture->getTexture()->setName(L"derivativeTexture");

		jacobianTexture->getTexture()->setName(L"jacobianTexture");
	}

	~MyRenderPass()
	{
		delete spectrumParamBuffer;

		delete spectrumCS;

		delete conjugateCS;

		delete displacementSpectrumCS;

		delete ifftCS;

		delete permutationCS;

		delete waveMergeCS;

		delete randomGaussTexture;

		delete tildeh0Texture;

		delete waveDataTexture;

		delete waveSpectrumTexture;

		delete tempTexture;

		delete Dy;

		delete Dx;

		delete Dz;

		delete Dyx;

		delete Dyz;

		delete Dxx;

		delete Dzz;

		delete Dxz;

		delete displacementTexture;

		delete derivativeTexture;

		delete jacobianTexture;
	}

protected:

	void recordCommand() override
	{
		calculateTimeDependentSpectrum();

		calculateDisplacementAndDerivative();

		float color[4] = { cosf(Graphics::getTimeElapsed()) * 0.5f + 0.5f,sinf(Graphics::getTimeElapsed()) * 0.5f + 0.5f,1.0f,1.0f };

		context->clearDefRenderTarget(color);
	}

private:

	static TextureRenderView* createTexture(const UINT& resolution, const DXGI_FORMAT& format)
	{
		return ResourceManager::createTextureRenderView(resolution, resolution, format, 1, 1, false, true, format, format, DXGI_FORMAT_UNKNOWN);
	}

	void calculateInitialSpectrum()
	{
		spectrumParamBuffer->update(&spectrumParam, sizeof(SpectrumParam));

		context->setPipelineState(spectrumState.Get());

		context->setCSConstants({
			tildeh0Texture->getUAVMipIndex(0),
			waveDataTexture->getUAVMipIndex(0),
			randomGaussTexture->getAllSRVIndex() }, 0);

		context->setCSConstantBuffer(spectrumParamBuffer);

		context->transitionResources();

		context->dispatch(textureResolution / 8 + 1, textureResolution / 8 + 1, 1);

		context->uavBarrier({
			tildeh0Texture->getTexture(),
			waveDataTexture->getTexture() });

		context->setPipelineState(conjugateState.Get());

		context->setCSConstants({
			waveSpectrumTexture->getUAVMipIndex(0),
			tildeh0Texture->getAllSRVIndex() }, 0);

		context->transitionResources();

		context->dispatch(textureResolution / 8, textureResolution / 8, 1);

		context->uavBarrier({
			waveSpectrumTexture->getTexture() });
	}

	void calculateTimeDependentSpectrum()
	{
		context->setPipelineState(displacementSpectrumState.Get());

		context->setCSConstants({
			Dy->getUAVMipIndex(0),
			Dx->getUAVMipIndex(0),
			Dz->getUAVMipIndex(0),
			Dyx->getUAVMipIndex(0),
			Dyz->getUAVMipIndex(0),
			Dxx->getUAVMipIndex(0),
			Dzz->getUAVMipIndex(0),
			Dxz->getUAVMipIndex(0),
			waveDataTexture->getAllSRVIndex(),
			waveSpectrumTexture->getAllSRVIndex() }, 0);

		context->transitionResources();

		context->dispatch(textureResolution / 8, textureResolution / 8, 1);

		context->uavBarrier({
			Dy->getTexture(),
			Dx->getTexture(),
			Dz->getTexture(),
			Dyx->getTexture(),
			Dyz->getTexture(),
			Dxx->getTexture(),
			Dzz->getTexture(),
			Dxz->getTexture() });
	}

	void calculateDisplacementAndDerivative()
	{
		ifftPermutation(Dy);

		ifftPermutation(Dx);

		ifftPermutation(Dz);

		ifftPermutation(Dyx);

		ifftPermutation(Dyz);

		ifftPermutation(Dxx);

		ifftPermutation(Dzz);

		ifftPermutation(Dxz);

		context->setPipelineState(waveMergeState.Get());

		context->setCSConstants({
			displacementTexture->getUAVMipIndex(0),
			derivativeTexture->getUAVMipIndex(0),
			jacobianTexture->getUAVMipIndex(0),
			Dy->getAllSRVIndex(),
			Dx->getAllSRVIndex(),
			Dz->getAllSRVIndex(),
			Dyx->getAllSRVIndex(),
			Dyz->getAllSRVIndex(),
			Dxx->getAllSRVIndex(),
			Dzz->getAllSRVIndex(),
			Dxz->getAllSRVIndex() }, 0);

		context->transitionResources();

		context->dispatch(textureResolution / 8, textureResolution / 8, 1);

		context->uavBarrier({
			displacementTexture->getTexture(),
			derivativeTexture->getTexture(),
			jacobianTexture->getTexture() });
	}

	void ifftPermutation(TextureRenderView* const inputTexture)
	{
		context->setPipelineState(ifftState.Get());

		context->setCSConstants({
			tempTexture->getUAVMipIndex(0),
			inputTexture->getAllSRVIndex() }, 0);

		context->transitionResources();

		context->dispatch(textureResolution, 1, 1);

		context->uavBarrier({
			tempTexture->getTexture() });

		context->setCSConstants({
			inputTexture->getUAVMipIndex(0),
			tempTexture->getAllSRVIndex() }, 0);

		context->transitionResources();

		context->dispatch(textureResolution, 1, 1);

		context->uavBarrier({
			inputTexture->getTexture() });

		context->setPipelineState(permutationState.Get());

		context->setCSConstants({
			inputTexture->getUAVMipIndex(0) }, 0);

		context->transitionResources();

		context->dispatch(textureResolution / 8, textureResolution / 8, 1);

		context->uavBarrier({
			inputTexture->getTexture() });
	}

	//do not change this!
	static constexpr UINT textureResolution = 1024;

	struct SpectrumParam
	{
		UINT mapResolution = textureResolution;
		float mapLength = 512.f;
		DirectX::XMFLOAT2 wind = { 20.f,0.f };
		float amplitude = 0.000001f;
		float gravity = 9.81f;
		DirectX::XMFLOAT2 padding0;
		DirectX::XMFLOAT4 padding1[14];
	} spectrumParam;

	ConstantBuffer* spectrumParamBuffer;

	Shader* spectrumCS;

	ComPtr<ID3D12PipelineState> spectrumState;

	Shader* conjugateCS;

	ComPtr<ID3D12PipelineState> conjugateState;

	Shader* displacementSpectrumCS;

	ComPtr<ID3D12PipelineState> displacementSpectrumState;

	Shader* ifftCS;

	ComPtr<ID3D12PipelineState> ifftState;

	//sign correction
	Shader* permutationCS;

	ComPtr<ID3D12PipelineState> permutationState;

	Shader* waveMergeCS;

	ComPtr<ID3D12PipelineState> waveMergeState;

	TextureRenderView* enviromentCube;

	//4 channel random gaussian distribution texture
	//mean 0 standard deviation 1
	TextureRenderView* randomGaussTexture;

	//(tildeh0(k))
	//x y
	TextureRenderView* tildeh0Texture;

	//k.x 1.0/length(K) k.z angularSpeed
	TextureRenderView* waveDataTexture;

	//(tildeh0(k), conj(tildeh0(-k)))
	//x y z w
	TextureRenderView* waveSpectrumTexture;

	//for ifft compute
	TextureRenderView* tempTexture;

	//Dy
	TextureRenderView* Dy;

	//following 2 textures are used for simulating choppy wave

	//Dx
	TextureRenderView* Dx;

	//Dz
	TextureRenderView* Dz;

	//following 5 textures are used for computing normals and jacobians

	//derivative dDy/dx
	TextureRenderView* Dyx;

	//derivative dDy/dz
	TextureRenderView* Dyz;

	//derivative dDx/dx
	TextureRenderView* Dxx;

	//derivative dDz/dz
	TextureRenderView* Dzz;

	//derivative dDx/dz
	TextureRenderView* Dxz;

	//Dx Dy Dz
	TextureRenderView* displacementTexture;

	//dDy/dx dDy/dz dDx/dx dDz/dz
	TextureRenderView* derivativeTexture;

	//J
	TextureRenderView* jacobianTexture;
};