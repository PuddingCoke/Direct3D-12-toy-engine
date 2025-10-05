﻿#pragma once

#include<Gear/Core/RenderTask.h>

#include<Gear/Core/Shader.h>

#include<Gear/Core/Resource/TextureRenderView.h>

#include<Gear/Core/Effect/BloomEffect.h>

#include<DirectXColors.h>

#include"WaveCascade.h"

class MyRenderTask :public RenderTask
{
public:

	MyRenderTask(FPSCamera* const camera) :
		vertices((gridSize + 1)* (gridSize + 1)),
		camera(camera),
		renderParamBuffer(ResourceManager::createConstantBuffer(sizeof(RenderParam), true)),
		spectrumParamBuffer{ ResourceManager::createConstantBuffer(sizeof(SpectrumParam), true),ResourceManager::createConstantBuffer(sizeof(SpectrumParam), true),ResourceManager::createConstantBuffer(sizeof(SpectrumParam), true) },
		cascade{ new WaveCascade(textureResolution,context),new WaveCascade(textureResolution,context),new WaveCascade(textureResolution,context) },
		textureCubePS(new Shader(Utils::getRootFolder() + L"TextureCubePS.cso")),
		gridDebugVS(new Shader(Utils::getRootFolder() + L"GridDebugVS.cso")),
		gridDebugPS(new Shader(Utils::getRootFolder() + L"GridDebugPS.cso")),
		oceanVS(new Shader(Utils::getRootFolder() + L"OceanVS.cso")),
		oceanHS(new Shader(Utils::getRootFolder() + L"OceanHS.cso")),
		oceanDS(new Shader(Utils::getRootFolder() + L"OceanDS.cso")),
		oceanPS(new Shader(Utils::getRootFolder() + L"OceanPS.cso")),
		spectrumCS(new Shader(Utils::getRootFolder() + L"SpectrumCS.cso")),
		conjugateCS(new Shader(Utils::getRootFolder() + L"ConjugateCS.cso")),
		displacementSpectrumCS(new Shader(Utils::getRootFolder() + L"DisplacementSpectrumCS.cso")),
		ifftCS(new Shader(Utils::getRootFolder() + L"IFFTCS.cso")),
		permutationCS(new Shader(Utils::getRootFolder() + L"PermutationCS.cso")),
		waveMergeCS(new Shader(Utils::getRootFolder() + L"WaveMergeCS.cso"))
	{
		spectrumParam[0].mapLength = lengthScale0;

		spectrumParam[1].mapLength = lengthScale1;

		spectrumParam[2].mapLength = lengthScale2;

		{
			D3D12_GRAPHICS_PIPELINE_STATE_DESC desc = PipelineState::getDefaultGraphicsDesc();
			desc.InputLayout = {};
			desc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
			desc.RasterizerState = PipelineState::rasterCullBack;
			desc.DepthStencilState.DepthEnable = false;
			desc.DepthStencilState.StencilEnable = false;
			desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
			desc.NumRenderTargets = 1;
			desc.RTVFormats[0] = DXGI_FORMAT_R16G16B16A16_FLOAT;
			desc.VS = Shader::textureCubeVS->getByteCode();
			desc.PS = textureCubePS->getByteCode();

			GraphicsDevice::get()->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(&textureCubeState));
		}

		{
			D3D12_INPUT_ELEMENT_DESC inputLayoutDesc[] =
			{
				{"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},
			};

			D3D12_GRAPHICS_PIPELINE_STATE_DESC desc = PipelineState::getDefaultGraphicsDesc();
			desc.InputLayout = { inputLayoutDesc,_countof(inputLayoutDesc) };
			desc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
			desc.RasterizerState = PipelineState::rasterCullBack;
			desc.DepthStencilState.DepthEnable = FALSE;
			desc.DepthStencilState.StencilEnable = FALSE;
			desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
			desc.NumRenderTargets = 1;
			desc.RTVFormats[0] = Graphics::backBufferFormat;
			desc.VS = gridDebugVS->getByteCode();
			desc.PS = gridDebugPS->getByteCode();

			GraphicsDevice::get()->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(&gridDebugState));
		}

		{
			D3D12_INPUT_ELEMENT_DESC inputLayoutDesc[] =
			{
				{"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},
			};

			D3D12_GRAPHICS_PIPELINE_STATE_DESC desc = PipelineState::getDefaultGraphicsDesc();
			desc.InputLayout = { inputLayoutDesc,_countof(inputLayoutDesc) };
			desc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
			desc.RasterizerState = PipelineState::rasterCullBack;
			desc.DepthStencilState = PipelineState::depthLess;
			desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_PATCH;
			desc.NumRenderTargets = 1;
			desc.RTVFormats[0] = DXGI_FORMAT_R16G16B16A16_FLOAT;
			desc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
			desc.VS = oceanVS->getByteCode();
			desc.HS = oceanHS->getByteCode();
			desc.DS = oceanDS->getByteCode();
			desc.PS = oceanPS->getByteCode();

			GraphicsDevice::get()->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(&oceanState));
		}

		PipelineState::createComputeState(&spectrumState, spectrumCS);

		PipelineState::createComputeState(&conjugateState, conjugateCS);

		PipelineState::createComputeState(&displacementSpectrumState, displacementSpectrumCS);

		PipelineState::createComputeState(&ifftState, ifftCS);

		PipelineState::createComputeState(&permutationState, permutationCS);

		PipelineState::createComputeState(&waveMergeState, waveMergeCS);

		tildeh0Texture = createTexture(textureResolution, DXGI_FORMAT_R32G32_FLOAT);

		tempTexture = createTexture(textureResolution, DXGI_FORMAT_R32G32_FLOAT);

		WaveCascade::spectrumState = spectrumState.Get();

		WaveCascade::conjugateState = conjugateState.Get();

		WaveCascade::displacementSpectrumState = displacementSpectrumState.Get();

		WaveCascade::ifftState = ifftState.Get();

		WaveCascade::permutationState = permutationState.Get();

		WaveCascade::waveMergeState = waveMergeState.Get();

		WaveCascade::tildeh0Texture = tildeh0Texture;

		WaveCascade::tempTexture = tempTexture;

		originTexture = ResourceManager::createTextureRenderView(Graphics::getWidth(), Graphics::getHeight(), DXGI_FORMAT_R16G16B16A16_FLOAT, 1, 1, false, true,
			DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_R16G16B16A16_FLOAT, DirectX::Colors::Black);

		depthTexture = ResourceManager::createTextureDepthView(Graphics::getWidth(), Graphics::getHeight(), DXGI_FORMAT_R32_TYPELESS, 1, 1, false, true);

		effect = new BloomEffect(context, Graphics::getWidth(), Graphics::getHeight(), resManager);

		vertexBuffer = resManager->createStructuredBufferView(sizeof(DirectX::XMFLOAT3), static_cast<UINT>(sizeof(DirectX::XMFLOAT3) * vertices.size()), false, false, true, true, true, vertices.data());

		{
			for (UINT y = 0; y < gridSize; y++)
			{
				for (UINT x = 0; x < gridSize; x++)
				{
					const UINT bottomLeft = y * (gridSize + 1) + x;

					const UINT bottomRight = bottomLeft + 1;

					const UINT topLeft = (y + 1) * (gridSize + 1) + x;

					const UINT topRight = topLeft + 1;

					indices.push_back(bottomRight);

					indices.push_back(bottomLeft);

					indices.push_back(topLeft);

					indices.push_back(topLeft);

					indices.push_back(topRight);

					indices.push_back(bottomRight);
				}
			}

			indexBuffer = resManager->createTypedBufferView(DXGI_FORMAT_R32_UINT, sizeof(UINT) * indices.size(), false, false, false, true, false, true, indices.data());
		}

		randomGaussTexture = resManager->createTextureRenderView(textureResolution, textureResolution, RandomDataType::GAUSS, true);

		WaveCascade::randomGaussTexture = randomGaussTexture;

		enviromentCube = resManager->createTextureCube(L"E:/Assets/Ocean/autumn_field_puresky_4k.exr", 1024, true);

		calculateInitialSpectrum();

		tempTexture->getTexture()->setName(L"tempTexture");

		randomGaussTexture->getTexture()->setName(L"randomGaussTexture");

		tildeh0Texture->getTexture()->setName(L"tildeh0Texture");

		originTexture->getTexture()->setName(L"originTexture");

		depthTexture->getTexture()->setName(L"depthTexture");

		oceanState->SetName(L"oceanState");

		effect->setExposure(0.59f);

		effect->setGamma(0.972f);

		effect->setSoftThreshold(0.85f);
	}

	~MyRenderTask()
	{
		delete vertexBuffer;

		delete indexBuffer;

		delete textureCubePS;

		delete gridDebugVS;

		delete gridDebugPS;

		delete oceanVS;

		delete oceanHS;

		delete oceanDS;

		delete oceanPS;

		delete spectrumCS;

		delete conjugateCS;

		delete displacementSpectrumCS;

		delete ifftCS;

		delete permutationCS;

		delete waveMergeCS;

		delete enviromentCube;

		delete randomGaussTexture;

		delete tildeh0Texture;

		delete tempTexture;

		delete originTexture;

		delete depthTexture;

		delete effect;

		delete renderParamBuffer;

		for (UINT i = 0; i < cascadeNum; i++)
		{
			delete spectrumParamBuffer[i];

			delete cascade[i];
		}
	}

	void imGUICall() override
	{
		effect->imGUICall();

		ImGui::Begin("RenderParam");
		ImGui::SliderFloat("Lod Scale", &renderParam.lodScale, 1.0f, 10.f);
		ImGui::SliderFloat("Sun Strength", &renderParam.sunStrength, 0.f, 100.f);
		ImGui::SliderFloat("Sun Theta", &renderParam.sunTheta, 0.f, Math::pi / 2.f);
		ImGui::SliderFloat("Specular Pow", &renderParam.specularPower, 0.f, 4096.f);
		ImGui::SliderFloat("Foam Bias", &renderParam.foamBias, 0.f, 5.f);
		ImGui::SliderFloat("Foam Scale", &renderParam.foamScale, 0.f, 1.f);
		ImGui::SliderFloat("Sun Direction", &renderParam.sunDirection, 0.f, Math::two_pi);
		ImGui::End();
	}

protected:

	void recordCommand() override
	{
		renderParam.exposure = effect->getExposure();

		renderParam.gamma = effect->getGamma();

		updateVertices();

		calculateDisplacementAndDerivative();

		renderSkyDomeAndOceanSurface();
	}

private:

	//do not change this!
	static constexpr UINT cascadeNum = 3;

	static constexpr float lengthScale0 = 250.f;

	static constexpr float lengthScale1 = 17.f;

	static constexpr float lengthScale2 = 5.f;

	//do not change this!
	static constexpr UINT textureResolution = 512;

	static constexpr UINT gridSize = 128;

	static TextureRenderView* createTexture(const UINT& resolution, const DXGI_FORMAT& format)
	{
		return ResourceManager::createTextureRenderView(resolution, resolution, format, 1, 1, false, true, format, format, DXGI_FORMAT_UNKNOWN);
	}

	void calculateInitialSpectrum()
	{
		const float boundary1 = 2.f * Math::pi / lengthScale1 * 12.f;

		const float boundary2 = 2.f * Math::pi / lengthScale2 * 12.f;

		{
			spectrumParam[0].cutoffLow = 0.0001f;

			spectrumParam[0].cutoffHigh = boundary1;
		}

		{
			spectrumParam[1].cutoffLow = boundary1;

			spectrumParam[1].cutoffHigh = boundary2;
		}

		{
			spectrumParam[2].cutoffLow = boundary2;

			spectrumParam[2].cutoffHigh = 9999.f;
		}

		for (UINT i = 0; i < cascadeNum; i++)
		{
			spectrumParamBuffer[i]->update(&spectrumParam[i], sizeof(SpectrumParam));

			cascade[i]->calculateInitialSpectrum(spectrumParamBuffer[i]);
		}
	}

	void calculateDisplacementAndDerivative()
	{
		for (UINT i = 0; i < cascadeNum; i++)
		{
			cascade[i]->calculateTimeDependentSpectrum();

			cascade[i]->calculateDisplacementAndDerivative();
		}
	}

	void renderSkyDomeAndOceanSurface()
	{
		/*context->setPipelineState(gridDebugState.Get());

		context->setViewportSimple(Graphics::getWidth(), Graphics::getHeight());

		context->setDefRenderTarget();

		context->setVertexBuffers(0, { vertexBuffer->getVertexBuffer() });

		context->setIndexBuffer(indexBuffer->getIndexBuffer());

		context->setTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		context->transitionResources();

		context->clearDefRenderTarget(DirectX::Colors::Black);

		context->draw(vertices.size(), 1, 0, 0);

		context->drawIndexed(indices.size(), 1, 0, 0, 0);*/

		renderParamBuffer->update(&renderParam, sizeof(RenderParam));

		context->setPipelineState(textureCubeState.Get());

		context->setViewportSimple(Graphics::getWidth(), Graphics::getHeight());

		context->setTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		context->setRenderTargets({ originTexture->getRTVMipHandle(0) }, {});

		context->setPSConstants({
			enviromentCube->getAllSRVIndex() }, 0);

		context->transitionResources();

		context->draw(36, 1, 0, 0);

		context->setPipelineState(oceanState.Get());

		context->setViewportSimple(Graphics::getWidth(), Graphics::getHeight());

		context->setTopology(D3D_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST);

		context->setVertexBuffers(0, { vertexBuffer->getVertexBuffer() });

		context->setIndexBuffer(indexBuffer->getIndexBuffer());

		const DepthStencilDesc depthStencilDesc = depthTexture->getDSVMipHandle(0);

		context->setRenderTargets({ originTexture->getRTVMipHandle(0) }, &depthStencilDesc);

		context->setDSConstants({
			cascade[0]->displacementTexture->getAllSRVIndex(),
			cascade[1]->displacementTexture->getAllSRVIndex(),
			cascade[2]->displacementTexture->getAllSRVIndex() }, 0);

		context->setDSConstantBuffer(renderParamBuffer);

		context->setPSConstants({
			cascade[0]->displacementTexture->getAllSRVIndex(),
			cascade[0]->derivativeTexture->getAllSRVIndex(),
			cascade[0]->jacobianTexture->getAllSRVIndex(),
			cascade[1]->displacementTexture->getAllSRVIndex(),
			cascade[1]->derivativeTexture->getAllSRVIndex(),
			cascade[1]->jacobianTexture->getAllSRVIndex(),
			cascade[2]->displacementTexture->getAllSRVIndex(),
			cascade[2]->derivativeTexture->getAllSRVIndex(),
			cascade[2]->jacobianTexture->getAllSRVIndex(),
			enviromentCube->getAllSRVIndex() }, 0);

		context->setPSConstantBuffer(renderParamBuffer);

		context->transitionResources();

		context->clearDepthStencil(depthTexture->getDSVMipHandle(0), D3D12_CLEAR_FLAG_DEPTH, 1.f, 0);

		context->drawIndexed(indices.size(), 1, 0, 0, 0);

		TextureRenderView* bloomTexture = effect->process(originTexture);

		blit(bloomTexture);
	}

	bool getMinMaxMatrix(DirectX::XMMATRIX* minMaxMatrix)
	{
		DirectX::XMVECTOR corners[8] = {
			{-1.f,-1.f,0.f,1.f},
			{-1.f,1.f,0.f,1.f},
			{1.f,1.f,0.f,1.f},
			{1.f,-1.f,0.f,1.f},
			{-1.f,-1.f,1.f,1.f},
			{-1.f,1.f,1.f,1.f},
			{1.f,1.f,1.f,1.f},
			{1.f,-1.f,1.f,1.f},
		};

		const UINT edges[12][2] = {
			{0,1},{1,2},{2,3},{3,0},
			{2,6},{3,7},{1,5},{0,4},
			{4,5},{5,6},{6,7},{7,4}
		};

		{
			const DirectX::XMMATRIX invViewProj = DirectX::XMMatrixInverse(nullptr, Camera::getView() * Camera::getProj());

			//transform corner point to world space
			for (UINT i = 0; i < 8; i++)
			{
				corners[i] = DirectX::XMVector3TransformCoord(corners[i], invViewProj);
			}
		}

		std::vector<DirectX::XMVECTOR> intersections;

		//get all intersections

		const float bound = 25.f;

		{
			const float a0 = bound;

			for (UINT i = 0; i < 12; i++)
			{
				const DirectX::XMVECTOR p0 = corners[edges[i][0]];

				const DirectX::XMVECTOR p1 = corners[edges[i][1]];

				const float y0 = DirectX::XMVectorGetY(p0);

				const float y1 = DirectX::XMVectorGetY(p1);

				if ((y0 - a0) * (y1 - a0) < 0.f)
				{
					const float t = (a0 - y0) / (y1 - y0);

					const DirectX::XMVECTOR intersection = DirectX::XMVectorLerp(p0, p1, t);

					intersections.push_back(intersection);
				}
			}
		}

		{
			const float a0 = -bound;

			for (UINT i = 0; i < 12; i++)
			{
				const DirectX::XMVECTOR p0 = corners[edges[i][0]];

				const DirectX::XMVECTOR p1 = corners[edges[i][1]];

				const float y0 = DirectX::XMVectorGetY(p0);

				const float y1 = DirectX::XMVectorGetY(p1);

				if ((y0 - a0) * (y1 - a0) < 0.f)
				{
					const float t = (a0 - y0) / (y1 - y0);

					const DirectX::XMVECTOR intersection = DirectX::XMVectorLerp(p0, p1, t);

					intersections.push_back(intersection);
				}
			}
		}

		//add interior point to intersections too
		for (UINT i = 0; i < 8; i++)
		{
			const DirectX::XMVECTOR p0 = corners[i];

			const float y0 = DirectX::XMVectorGetY(p0);

			if (y0<bound && y0>-bound)
			{
				intersections.push_back(p0);
			}
		}

		//project all points to y=0 plane
		for (UINT i = 0; i < intersections.size(); i++)
		{
			intersections[i] = DirectX::XMVectorSetY(intersections[i], 0.f);
		}

		DirectX::XMVECTOR aimPoint0;

		DirectX::XMVECTOR aimPoint1;

		{
			DirectX::XMVECTOR forward = camera->getLookDir();

			if (DirectX::XMVectorGetY(forward) > 0.f)
			{
				forward = DirectX::XMVectorSetY(forward, -DirectX::XMVectorGetY(forward));
			}

			{
				const DirectX::XMVECTOR p0 = camera->getEyePos();

				const DirectX::XMVECTOR p1 = DirectX::XMVectorAdd(p0, forward);

				const float y0 = DirectX::XMVectorGetY(p0);

				const float y1 = DirectX::XMVectorGetY(p1);

				const float t = -y0 / (y1 - y0);

				aimPoint0 = DirectX::XMVectorLerp(p0, p1, t);
			}
		}

		const float af = fabsf(DirectX::XMVectorGetY(camera->getLookDir()));

		aimPoint1 = DirectX::XMVectorAdd(camera->getEyePos(), DirectX::XMVectorScale(camera->getLookDir(), 10.f));

		aimPoint1 = DirectX::XMVectorSetY(aimPoint1, 0.f);

		aimPoint0 = DirectX::XMVectorLerp(aimPoint1, aimPoint0, af);

		const DirectX::XMMATRIX viewMatrix = DirectX::XMMatrixLookAtLH(camera->getEyePos(), aimPoint0, camera->getUpVector());

		{
			const DirectX::XMMATRIX viewProj = viewMatrix * Camera::getProj();

			for (UINT i = 0; i < intersections.size(); i++)
			{
				intersections[i] = DirectX::XMVector3TransformCoord(intersections[i], viewProj);
			}
		}

		float x_min = DirectX::XMVectorGetX(intersections[0]);

		float x_max = DirectX::XMVectorGetX(intersections[0]);

		float y_min = DirectX::XMVectorGetY(intersections[0]);

		float y_max = DirectX::XMVectorGetY(intersections[0]);

		for (UINT i = 1; i < intersections.size(); i++)
		{
			const float x = DirectX::XMVectorGetX(intersections[i]);

			const float y = DirectX::XMVectorGetY(intersections[i]);

			if (x > x_max) x_max = x;
			if (x < x_min) x_min = x;
			if (y > y_max) y_max = y;
			if (y < y_min) y_min = y;
		}

		DirectX::XMMATRIX pack(x_max - x_min, 0, 0, x_min,
			0, y_max - y_min, 0, y_min,
			0, 0, 1, 0,
			0, 0, 0, 1);

		pack = DirectX::XMMatrixTranspose(pack);

		*minMaxMatrix = pack * DirectX::XMMatrixInverse(nullptr, viewMatrix * Camera::getProj());

		return true;
	}

	DirectX::XMFLOAT4 getWorldPos(const float u, const float v, const DirectX::XMMATRIX& m)
	{
		DirectX::XMVECTOR origin = { u,v,0.f,1.f };

		DirectX::XMVECTOR direction = { u,v,1.f,1.f };

		origin = DirectX::XMVector4Transform(origin, m);

		direction = DirectX::XMVector4Transform(direction, m);

		direction = DirectX::XMVectorSubtract(direction, origin);

		const float l = -DirectX::XMVectorGetY(origin) / DirectX::XMVectorGetY(direction);

		const DirectX::XMVECTOR worldPos = DirectX::XMVectorAdd(origin, DirectX::XMVectorScale(direction, l));

		DirectX::XMFLOAT4 flt;

		DirectX::XMStoreFloat4(&flt, worldPos);

		return flt;
	}

	void updateVertices()
	{
		DirectX::XMMATRIX range;

		getMinMaxMatrix(&range);

		//left bottom
		const DirectX::XMFLOAT4 corners0 = getWorldPos(0.f, 0.f, range);

		//right bottom
		const DirectX::XMFLOAT4 corners1 = getWorldPos(1.f, 0.f, range);

		//left top
		const DirectX::XMFLOAT4 corners2 = getWorldPos(0.f, 1.f, range);

		//right top
		const DirectX::XMFLOAT4 corners3 = getWorldPos(1.f, 1.f, range);

		for (UINT y = 0; y < gridSize + 1; y++)
		{
			for (UINT x = 0; x < gridSize + 1; x++)
			{
				const float u = (float)x / (float)gridSize;

				const float v = (float)y / (float)gridSize;

				DirectX::XMFLOAT4 result;

				result.x = (1.f - v) * ((1.f - u) * corners0.x + u * corners1.x) + v * ((1.f - u) * corners2.x + u * corners3.x);
				result.z = (1.f - v) * ((1.f - u) * corners0.z + u * corners1.z) + v * ((1.f - u) * corners2.z + u * corners3.z);
				result.w = (1.f - v) * ((1.f - u) * corners0.w + u * corners1.w) + v * ((1.f - u) * corners2.w + u * corners3.w);

				const float divide = 1.f / result.w;

				result.x *= divide;

				result.z *= divide;

				const UINT writeCoord = y * (gridSize + 1) + x;

				vertices[writeCoord] = { result.x,0.f,result.z };
			}
		}

		context->updateBuffer(vertexBuffer, vertices.data(), static_cast<UINT>(sizeof(DirectX::XMFLOAT3) * vertices.size()));
	}

	struct SpectrumParam
	{
		const UINT mapResolution = textureResolution;
		float mapLength;
		const DirectX::XMFLOAT2 wind = { 12.f * cosf(0.93f + Math::half_pi),12.f * sinf(0.93f + Math::half_pi) };
		const float amplitude = 0.000002f;
		const float gravity = 9.81f;
		float cutoffLow;
		float cutoffHigh;
		const DirectX::XMFLOAT4 padding1[14] = {};
	} spectrumParam[cascadeNum];

	struct RenderParam
	{
		float lodScale = 3.f;
		float lengthScale0 = MyRenderTask::lengthScale0;
		float lengthScale1 = MyRenderTask::lengthScale1;
		float lengthScale2 = MyRenderTask::lengthScale2;
		float sunStrength = 0.548f;
		float sunTheta = 0.143f;
		float specularPower = 256.f;
		float foamBias = 1.6f;
		float foamScale = 1.f / 1.6f;
		float sunDirection = 0.93f;
		float exposure;
		float gamma;
		DirectX::XMFLOAT4 padding1[13] = {};
	} renderParam;

	BufferView* vertexBuffer;

	BufferView* indexBuffer;

	Shader* textureCubePS;

	ComPtr<ID3D12PipelineState> textureCubeState;

	Shader* gridDebugVS;

	Shader* gridDebugPS;

	ComPtr<ID3D12PipelineState> gridDebugState;

	Shader* oceanVS;

	Shader* oceanHS;

	Shader* oceanDS;

	Shader* oceanPS;

	ComPtr<ID3D12PipelineState> oceanState;

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

	//intermediate texture for ifft compute
	TextureRenderView* tempTexture;

	TextureRenderView* originTexture;

	TextureDepthView* depthTexture;

	BloomEffect* effect;

	ConstantBuffer* renderParamBuffer;

	ConstantBuffer* spectrumParamBuffer[cascadeNum];

	WaveCascade* cascade[cascadeNum];

	FPSCamera* camera;

	std::vector<DirectX::XMFLOAT3> vertices;

	std::vector<UINT> indices;
};