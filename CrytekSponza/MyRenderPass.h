#pragma once

#include<Gear/Core/RenderPass.h>
#include<Gear/Core/Shader.h>

#include<Gear/Core/Effect/BloomEffect.h>

#include<DirectXColors.h>

#include"Scene.h"

class MyRenderPass :public RenderPass
{
public:

	MyRenderPass() :
		gPosition(ResourceManager::createTextureRenderTarget(Graphics::getWidth(), Graphics::getHeight(), DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 1, false, true,
			DXGI_FORMAT_R32G32B32A32_FLOAT, DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_R32G32B32A32_FLOAT, DirectX::Colors::Transparent)),
		gNormalSpecular(ResourceManager::createTextureRenderTarget(Graphics::getWidth(), Graphics::getHeight(), DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 1, false, true,
			DXGI_FORMAT_R32G32B32A32_FLOAT, DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_R32G32B32A32_FLOAT, DirectX::Colors::Transparent)),
		gBaseColor(ResourceManager::createTextureRenderTarget(Graphics::getWidth(), Graphics::getHeight(), DXGI_FORMAT_R8G8B8A8_UNORM, 1, 1, false, true,
			DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_R8G8B8A8_UNORM, DirectX::Colors::Transparent)),
		depthTexture(ResourceManager::createTextureDepthStencil(Graphics::getWidth(), Graphics::getHeight(), DXGI_FORMAT_R32_TYPELESS, 1, 1, false, true)),
		shadowTexture(ResourceManager::createTextureDepthStencil(shadowTextureResolution, shadowTextureResolution, DXGI_FORMAT_R32_TYPELESS, 1, 1, false, true)),
		originTexture(ResourceManager::createTextureRenderTarget(Graphics::getWidth(), Graphics::getHeight(), DXGI_FORMAT_R16G16B16A16_FLOAT, 1, 1, false, true,
			DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_R16G16B16A16_FLOAT, DirectX::Colors::Black)),
		irradianceVolumeBuffer(ResourceManager::createConstantBuffer(sizeof(IrradianceVolume), true)),
		shadowVS(new Shader(Utils::getRootFolder() + "ShadowVS.cso")),
		deferredVShader(new Shader(Utils::getRootFolder() + "DeferredVShader.cso")),
		deferredPShader(new Shader(Utils::getRootFolder() + "DeferredPShader.cso")),
		deferredFinal(new Shader(Utils::getRootFolder() + "DeferredFinal.cso")),
		sunAngle(Math::half_pi - 0.01f)
	{
		shadowTexture->getTexture()->getResource()->SetName(L"shadow texture");

		{
			D3D12_GRAPHICS_PIPELINE_STATE_DESC desc = PipelineState::getDefaultGraphicsDesc();
			desc.InputLayout = { inputDesc,_countof(inputDesc) };
			desc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
			desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
			desc.RasterizerState = States::rasterShadow;
			desc.DepthStencilState = States::depthLess;
			desc.NumRenderTargets = 0;
			desc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
			desc.VS = shadowVS->getByteCode();

			GraphicsDevice::get()->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(&shadowPipelineState));
		}

		{
			D3D12_GRAPHICS_PIPELINE_STATE_DESC desc = PipelineState::getDefaultGraphicsDesc();
			desc.InputLayout = { inputDesc,_countof(inputDesc) };
			desc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
			desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
			desc.RasterizerState = States::rasterCullBack;
			desc.DepthStencilState = States::depthLess;
			desc.NumRenderTargets = 3;
			desc.RTVFormats[0] = DXGI_FORMAT_R32G32B32A32_FLOAT;
			desc.RTVFormats[1] = DXGI_FORMAT_R32G32B32A32_FLOAT;
			desc.RTVFormats[2] = DXGI_FORMAT_R8G8B8A8_UNORM;
			desc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
			desc.VS = deferredVShader->getByteCode();
			desc.PS = deferredPShader->getByteCode();

			GraphicsDevice::get()->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(&deferredPipelineState));
		}

		{
			D3D12_GRAPHICS_PIPELINE_STATE_DESC desc = PipelineState::getDefaultFullScreenState();
			desc.NumRenderTargets = 1;
			desc.RTVFormats[0] = DXGI_FORMAT_R16G16B16A16_FLOAT;
			desc.PS = deferredFinal->getByteCode();

			GraphicsDevice::get()->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(&deferredFinalPipelineState));
		}

		{
			D3D12_GRAPHICS_PIPELINE_STATE_DESC desc = PipelineState::getDefaultFullScreenState();
			desc.NumRenderTargets = 1;
			desc.RTVFormats[0] = Graphics::BackBufferFormat;
			desc.PS = Shader::fullScreenPS->getByteCode();

			GraphicsDevice::get()->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(&fullScreenBlitState));
		}

		begin();

		effect = new BloomEffect(context, Graphics::getWidth(), Graphics::getHeight(), resManager);

		scene = new Scene(assetPath + "/sponza.dae", resManager);

		updateShadow();

		irradianceVolumeBuffer->update(&irradianceVolume, sizeof(IrradianceVolume));

		end();

		RenderEngine::get()->submitRenderPass(this);
	}

	~MyRenderPass()
	{
		delete scene;

		delete irradianceVolumeBuffer;

		delete gPosition;
		delete gNormalSpecular;
		delete gBaseColor;
		delete depthTexture;
		delete shadowTexture;
		delete originTexture;

		delete shadowVS;
		delete deferredVShader;
		delete deferredPShader;
		delete deferredFinal;

		delete effect;
	}

protected:

	static constexpr UINT shadowTextureResolution = 4096;

	void updateShadow()
	{
		const float xSize = 183;
		const float ySize = 130;
		const float distance = 260.f;
		const DirectX::XMVECTOR offset = { 6.5f,0.f,0.f };

		irradianceVolume.lightDir = { 0.f,sinf(sunAngle),cosf(sunAngle),0.f };

		irradianceVolume.lightColor = DirectX::Colors::White;

		irradianceVolume.lightDir = DirectX::XMVector3Normalize(irradianceVolume.lightDir);

		const DirectX::XMVECTOR lightCamPos = DirectX::XMVectorAdd(DirectX::XMVectorScale(irradianceVolume.lightDir, distance), offset);
		const DirectX::XMMATRIX lightProjMat = DirectX::XMMatrixOrthographicLH(xSize, ySize, 1.f, 512.f);
		const DirectX::XMMATRIX lightViewMat = DirectX::XMMatrixLookAtLH(lightCamPos, offset, { 0.f,1.f,0.f });
		const DirectX::XMMATRIX lightMat = DirectX::XMMatrixTranspose(lightViewMat * lightProjMat);

		irradianceVolume.lightViewProj = lightMat;

		const DepthStencilDesc dsDesc = shadowTexture->getDSVMipHandle(0);

		context->setRenderTargets({}, &dsDesc);

		context->setViewport(shadowTextureResolution, shadowTextureResolution);

		context->setScissorRect(0, 0, shadowTextureResolution, shadowTextureResolution);

		context->setVSConstantBuffer(irradianceVolumeBuffer);

		context->transitionResources();

		context->clearDepthStencil(dsDesc, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0);

		scene->render(context, shadowPipelineState.Get());
	}

	void recordCommand() override
	{
		context->setViewport(Graphics::getWidth(), Graphics::getHeight());

		context->setScissorRect(0, 0, Graphics::getWidth(), Graphics::getHeight());

		const DepthStencilDesc dsDesc = depthTexture->getDSVMipHandle(0);

		context->setRenderTargets({
			gPosition->getRTVMipHandle(0),
			gNormalSpecular->getRTVMipHandle(0),
			gBaseColor->getRTVMipHandle(0)
			}, &dsDesc);

		context->transitionResources();

		context->clearRenderTarget(gPosition->getRTVMipHandle(0), DirectX::Colors::Transparent);

		context->clearRenderTarget(gNormalSpecular->getRTVMipHandle(0), DirectX::Colors::Transparent);

		context->clearRenderTarget(gBaseColor->getRTVMipHandle(0), DirectX::Colors::Transparent);

		context->clearDepthStencil(dsDesc, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0);

		scene->render(context, deferredPipelineState.Get());

		context->setPipelineState(deferredFinalPipelineState.Get());

		context->setViewport(Graphics::getWidth(), Graphics::getHeight());

		context->setScissorRect(0, 0, Graphics::getWidth(), Graphics::getHeight());

		context->setTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		context->setRenderTargets({ originTexture->getRTVMipHandle(0) });

		context->setPSConstants({ 
			gPosition->getAllSRVIndex(),
			gNormalSpecular->getAllSRVIndex(),
			gBaseColor->getAllSRVIndex(),
			shadowTexture->getAllDepthIndex() 
			}, 0);

		context->setPSConstantBuffer(irradianceVolumeBuffer);

		context->transitionResources();

		context->clearRenderTarget(originTexture->getRTVMipHandle(0), DirectX::Colors::Black);

		context->draw(3, 1, 0, 0);

		TextureRenderTarget* bloomTexture = effect->process(originTexture);

		context->setPipelineState(fullScreenBlitState.Get());

		context->setViewport(Graphics::getWidth(), Graphics::getHeight());

		context->setScissorRect(0, 0, Graphics::getWidth(), Graphics::getHeight());

		context->setTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		context->setDefRenderTarget();

		context->setPSConstants({ bloomTexture->getAllSRVIndex() }, 0);

		context->transitionResources();

		context->clearDefRenderTarget(DirectX::Colors::Black);

		context->draw(3, 1, 0, 0);
	}

	Scene* scene;

	struct IrradianceVolume
	{
		DirectX::XMVECTOR lightDir;
		DirectX::XMVECTOR lightColor;
		DirectX::XMMATRIX lightViewProj;
		DirectX::XMFLOAT3 start = { -142.f,-16.f,-74.f };
		float spacing = 18.2f;
		DirectX::XMUINT3 count = { 17,9,12 };
		float irradianceDistanceBias = 0.8f;
		float irradianceVarianceBias = 0.f;
		float irradianceChebyshevBias = -1.0f;
		DirectX::XMFLOAT2 padding0;
		DirectX::XMFLOAT4 padding1[7];
	} irradianceVolume;

	ConstantBuffer* irradianceVolumeBuffer;

	float sunAngle;

	TextureRenderTarget* gPosition;

	TextureRenderTarget* gNormalSpecular;

	TextureRenderTarget* gBaseColor;

	TextureDepthStencil* depthTexture;

	TextureDepthStencil* shadowTexture;

	TextureRenderTarget* originTexture;

	ComPtr<ID3D12PipelineState> shadowPipelineState;

	ComPtr<ID3D12PipelineState> deferredPipelineState;

	ComPtr<ID3D12PipelineState> deferredFinalPipelineState;

	ComPtr<ID3D12PipelineState> fullScreenBlitState;

	Shader* shadowVS;

	Shader* deferredVShader;

	Shader* deferredPShader;

	Shader* deferredFinal;

	BloomEffect* effect;

};