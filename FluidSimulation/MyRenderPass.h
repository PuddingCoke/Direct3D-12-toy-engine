#pragma once

#include<Gear/Core/RenderPass.h>

#include<Gear/Core/Effect/BloomEffect.h>

#include<Gear/Utils/Color.h>

#include<DirectXColors.h>

class MyRenderPass :public RenderPass
{
public:

	MyRenderPass() :
		colorUpdateTimer(1.f),
		splatVelocityCS(new Shader(Utils::getRootFolder() + "SplatVelocityCS.cso")),
		splatColorCS(new Shader(Utils::getRootFolder() + "SplatColorCS.cso")),
		divergenceCS(new Shader(Utils::getRootFolder() + "DivergenceCS.cso")),
		pressureResetCS(new Shader(Utils::getRootFolder() + "PressureResetCS.cso")),
		pressureCS(new Shader(Utils::getRootFolder() + "PressureCS.cso")),
		gradientSubtractCS(new Shader(Utils::getRootFolder() + "GradientSubtractCS.cso")),
		velocityAdvectionCS(new Shader(Utils::getRootFolder() + "VelocityAdvectionCS.cso")),
		colorAdvectionCS(new Shader(Utils::getRootFolder() + "ColorAdvectionCS.cso")),
		fluidFinalPS(new Shader(Utils::getRootFolder() + "FluidFinalPS.cso"))
	{
		const DirectX::XMUINT2 simRes = { Graphics::getWidth() / config.resolutionFactor,Graphics::getHeight() / config.resolutionFactor };

		velocityTex = new SwapTexture([=] {return ResourceManager::createTextureRenderView(simRes.x, simRes.y, DXGI_FORMAT_R32G32_FLOAT, 1, 1, false, true,
			DXGI_FORMAT_R32G32_FLOAT, DXGI_FORMAT_R32G32_FLOAT, DXGI_FORMAT_UNKNOWN); });

		velocityTex->read()->getTexture()->getResource()->SetName(L"Velocity Texture (0)");

		velocityTex->write()->getTexture()->getResource()->SetName(L"Velocity Texture (1)");

		colorTex = new SwapTexture([=] {return ResourceManager::createTextureRenderView(Graphics::getWidth(), Graphics::getHeight(), DXGI_FORMAT_R16G16B16A16_FLOAT, 1, 1, false, true,
			DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_UNKNOWN); });

		colorTex->read()->getTexture()->getResource()->SetName(L"Color Texture (0)");

		colorTex->write()->getTexture()->getResource()->SetName(L"Color Texture (1)");

		divergenceTex = ResourceManager::createTextureRenderView(simRes.x, simRes.y, DXGI_FORMAT_R32_FLOAT, 1, 1, false, true,
			DXGI_FORMAT_R32_FLOAT, DXGI_FORMAT_R32_FLOAT, DXGI_FORMAT_UNKNOWN);

		divergenceTex->getTexture()->getResource()->SetName(L"Divergence Texture");

		pressureTex = new SwapTexture([=] {return ResourceManager::createTextureRenderView(simRes.x, simRes.y, DXGI_FORMAT_R32_FLOAT, 1, 1, false, true,
			DXGI_FORMAT_R32_FLOAT, DXGI_FORMAT_R32_FLOAT, DXGI_FORMAT_UNKNOWN); });

		pressureTex->read()->getTexture()->getResource()->SetName(L"Pressure Texture (0)");

		pressureTex->write()->getTexture()->getResource()->SetName(L"Pressure Texture (1)");

		curlTex = ResourceManager::createTextureRenderView(simRes.x, simRes.y, DXGI_FORMAT_R32_FLOAT, 1, 1, false, true,
			DXGI_FORMAT_R32_FLOAT, DXGI_FORMAT_R32_FLOAT, DXGI_FORMAT_UNKNOWN);

		originTexture = ResourceManager::createTextureRenderView(Graphics::getWidth(), Graphics::getHeight(), DXGI_FORMAT_R16G16B16A16_FLOAT, 1, 1, false, true,
			DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_R16G16B16A16_FLOAT);

		simulationParamBuffer = ResourceManager::createConstantBuffer(sizeof(SimulationParam), true);

		simulationParam.colorTexelSize = DirectX::XMFLOAT2(1.f / Graphics::getWidth(), 1.f / Graphics::getHeight());

		simulationParam.simTexelSize = DirectX::XMFLOAT2(1.f / simRes.x, 1.f / simRes.y);

		simulationParam.colorTextureSize = DirectX::XMUINT2(Graphics::getWidth(), Graphics::getHeight());

		simulationParam.simTextureSize = DirectX::XMUINT2(simRes.x, simRes.y);

		simulationParam.colorDissipationSpeed = config.colorDissipationSpeed;

		simulationParam.velocityDissipationSpeed = config.velocityDissipationSpeed;

		simulationParam.curlIntensity = config.curlIntensity;

		simulationParam.splatRadius = config.splatRadius / 100.f * Graphics::getAspectRatio();

		{
			D3D12_COMPUTE_PIPELINE_STATE_DESC desc = PipelineState::getDefaultComputeDesc();
			desc.CS = splatVelocityCS->getByteCode();

			GraphicsDevice::get()->CreateComputePipelineState(&desc, IID_PPV_ARGS(&splatVelocityState));
		}

		{
			D3D12_COMPUTE_PIPELINE_STATE_DESC desc = PipelineState::getDefaultComputeDesc();
			desc.CS = splatColorCS->getByteCode();

			GraphicsDevice::get()->CreateComputePipelineState(&desc, IID_PPV_ARGS(&splatColorState));
		}

		{
			D3D12_COMPUTE_PIPELINE_STATE_DESC desc = PipelineState::getDefaultComputeDesc();
			desc.CS = divergenceCS->getByteCode();

			GraphicsDevice::get()->CreateComputePipelineState(&desc, IID_PPV_ARGS(&divergenceState));
		}

		{
			D3D12_COMPUTE_PIPELINE_STATE_DESC desc = PipelineState::getDefaultComputeDesc();
			desc.CS = pressureResetCS->getByteCode();

			GraphicsDevice::get()->CreateComputePipelineState(&desc, IID_PPV_ARGS(&pressureResetState));
		}

		{
			D3D12_COMPUTE_PIPELINE_STATE_DESC desc = PipelineState::getDefaultComputeDesc();
			desc.CS = pressureCS->getByteCode();

			GraphicsDevice::get()->CreateComputePipelineState(&desc, IID_PPV_ARGS(&pressureState));
		}

		{
			D3D12_COMPUTE_PIPELINE_STATE_DESC desc = PipelineState::getDefaultComputeDesc();
			desc.CS = gradientSubtractCS->getByteCode();

			GraphicsDevice::get()->CreateComputePipelineState(&desc, IID_PPV_ARGS(&gradientSubtractState));
		}

		{
			D3D12_COMPUTE_PIPELINE_STATE_DESC desc = PipelineState::getDefaultComputeDesc();
			desc.CS = velocityAdvectionCS->getByteCode();

			GraphicsDevice::get()->CreateComputePipelineState(&desc, IID_PPV_ARGS(&velocityAdvectionState));
		}

		{
			D3D12_COMPUTE_PIPELINE_STATE_DESC desc = PipelineState::getDefaultComputeDesc();
			desc.CS = colorAdvectionCS->getByteCode();

			GraphicsDevice::get()->CreateComputePipelineState(&desc, IID_PPV_ARGS(&colorAdvectionState));
		}

		{
			D3D12_GRAPHICS_PIPELINE_STATE_DESC desc = getDefaultGraphicsPipelineDesc();
			desc.PS = fluidFinalPS->getByteCode();
			desc.RTVFormats[0] = originTexture->getTexture()->getFormat();

			GraphicsDevice::get()->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(&fluidFinalState));
		}

		{
			D3D12_GRAPHICS_PIPELINE_STATE_DESC desc = getDefaultGraphicsPipelineDesc();
			desc.PS = Shader::fullScreenPS->getByteCode();
			desc.RTVFormats[0] = Graphics::BackBufferFormat;

			GraphicsDevice::get()->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(&fullScreenState));
		}

		begin();

		effect = new BloomEffect(context, Graphics::getWidth(), Graphics::getHeight(), resManager);

		end();

		RenderEngine::get()->submitRenderPass(this);

		effect->setThreshold(0.f);
	}

	//just configure
	//PS
	//RTVFormats[0]
	D3D12_GRAPHICS_PIPELINE_STATE_DESC getDefaultGraphicsPipelineDesc()
	{
		D3D12_GRAPHICS_PIPELINE_STATE_DESC desc = {};
		desc.InputLayout = {};
		desc.pRootSignature = GlobalRootSignature::getGraphicsRootSignature()->get();
		desc.VS = Shader::fullScreenVS->getByteCode();
		desc.RasterizerState = States::rasterCullBack;
		desc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
		desc.DepthStencilState.DepthEnable = FALSE;
		desc.DepthStencilState.StencilEnable = FALSE;
		desc.SampleMask = UINT_MAX;
		desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		desc.NumRenderTargets = 1;
		desc.SampleDesc.Count = 1;

		return desc;
	}

	UINT countA = 0;

	~MyRenderPass()
	{
		delete effect;

		delete colorTex;
		delete velocityTex;
		delete curlTex;
		delete divergenceTex;
		delete pressureTex;
		delete originTexture;

		delete simulationParamBuffer;

		delete splatVelocityCS;
		delete splatColorCS;
		delete divergenceCS;
		delete pressureResetCS;
		delete pressureCS;
		delete gradientSubtractCS;
		delete velocityAdvectionCS;
		delete colorAdvectionCS;
		delete fluidFinalPS;
	}

	void recordCommand() override
	{
		const DirectX::XMFLOAT2 pos =
		{
			(float)Mouse::getX() / Graphics::getWidth(),
			(float)(Graphics::getHeight() - Mouse::getY()) / Graphics::getHeight()
		};

		const DirectX::XMFLOAT2 posDelta =
		{
			(pos.x - simulationParam.pos.x) * config.splatForce,
			((pos.y - simulationParam.pos.y) / Graphics::getAspectRatio()) * config.splatForce
		};

		simulationParam.pos = pos;
		simulationParam.posDelta = posDelta;

		if (colorUpdateTimer.update(Graphics::getDeltaTime() * config.colorChangeSpeed))
		{
			Color c = Color::HSVtoRGB({ Random::Float(),1.f,1.f });
			simulationParam.splatColor = { c.r,c.g,c.b,1.f };
		}

		simulationParamBuffer->update(&simulationParam, sizeof(SimulationParam));

		context->setGlobalConstantBuffer(simulationParamBuffer);
		context->setTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		if (Mouse::onMove() && Mouse::getLeftDown())
		{
			context->setPipelineState(splatVelocityState.Get());
			context->setCSConstants({ velocityTex->read()->getAllSRVIndex(),velocityTex->write()->getUAVMipIndex(0) }, 0);
			context->transitionResources();
			context->dispatch(velocityTex->width / 16, velocityTex->height / 9, 1);
			context->uavBarrier({ velocityTex->write()->getTexture() });
			velocityTex->swap();

			context->setPipelineState(splatColorState.Get());
			context->setCSConstants({ colorTex->read()->getAllSRVIndex(),colorTex->write()->getUAVMipIndex(0) }, 0);
			context->transitionResources();
			context->dispatch(colorTex->width / 16, colorTex->height / 9, 1);
			context->uavBarrier({ colorTex->write()->getTexture() });
			colorTex->swap();
		}

		context->setPipelineState(divergenceState.Get());
		context->setCSConstants({ velocityTex->read()->getAllSRVIndex(),divergenceTex->getUAVMipIndex(0) }, 0);
		context->transitionResources();
		context->dispatch(divergenceTex->getTexture()->getWidth() / 16, divergenceTex->getTexture()->getHeight() / 9, 1);
		context->uavBarrier({ divergenceTex->getTexture() });

		context->setPipelineState(pressureResetState.Get());
		context->setCSConstants({ pressureTex->write()->getUAVMipIndex(0) }, 0);
		context->transitionResources();
		context->dispatch(pressureTex->width / 16, pressureTex->height / 9, 1);
		context->uavBarrier({ pressureTex->write()->getTexture() });
		pressureTex->swap();

		context->setPipelineState(pressureState.Get());
		for (UINT i = 0; i < config.pressureIteraion; i++)
		{
			context->setCSConstants({ divergenceTex->getAllSRVIndex(),pressureTex->read()->getAllSRVIndex(),pressureTex->write()->getUAVMipIndex(0) }, 0);
			context->transitionResources();
			context->dispatch(pressureTex->width / 16, pressureTex->height / 9, 1);
			context->uavBarrier({ pressureTex->write()->getTexture() });
			pressureTex->swap();
		}

		context->setPipelineState(gradientSubtractState.Get());
		context->setCSConstants({ pressureTex->read()->getAllSRVIndex(),velocityTex->read()->getAllSRVIndex(),velocityTex->write()->getUAVMipIndex(0) }, 0);
		context->transitionResources();
		context->dispatch(velocityTex->width / 16, velocityTex->height / 9, 1);
		context->uavBarrier({ velocityTex->write()->getTexture() });
		velocityTex->swap();

		context->setPipelineState(velocityAdvectionState.Get());
		context->setCSConstants({ velocityTex->read()->getAllSRVIndex(),velocityTex->write()->getUAVMipIndex(0) }, 0);
		context->transitionResources();
		context->dispatch(velocityTex->width / 16, velocityTex->height / 9, 1);
		context->uavBarrier({ velocityTex->write()->getTexture() });
		velocityTex->swap();

		context->setPipelineState(colorAdvectionState.Get());
		context->setCSConstants({ velocityTex->read()->getAllSRVIndex(),colorTex->read()->getAllSRVIndex(),colorTex->write()->getUAVMipIndex(0) }, 0);
		context->transitionResources();
		context->dispatch(colorTex->width / 16, colorTex->height / 9, 1);
		context->uavBarrier({ colorTex->write()->getTexture() });
		colorTex->swap();

		context->setViewport(colorTex->width, colorTex->height);
		context->setScissorRect(0, 0, colorTex->width, colorTex->height);

		context->setPipelineState(fluidFinalState.Get());
		context->setRenderTargets({ originTexture->getRTVMipHandle(0) }, {});
		context->setPSConstants({ colorTex->read()->getAllSRVIndex() }, 0);
		context->transitionResources();
		context->draw(3, 1, 0, 0);

		TextureRenderView* const outputTexture = effect->process(originTexture);

		context->setPipelineState(fullScreenState.Get());
		context->setDefRenderTarget();
		context->clearDefRenderTarget(DirectX::Colors::Black);
		context->setPSConstants({ outputTexture->getAllSRVIndex() }, 0);
		context->transitionResources();
		context->draw(3, 1, 0, 0);
	}

private:

	BloomEffect* effect;

	SwapTexture* colorTex;

	SwapTexture* velocityTex;

	TextureRenderView* curlTex;

	TextureRenderView* divergenceTex;

	SwapTexture* pressureTex;

	TextureRenderView* originTexture;

	struct Config
	{
		float colorChangeSpeed = 10.f;//颜色改变速度
		float colorDissipationSpeed = 0.5f;//颜色消散速度
		float velocityDissipationSpeed = 0.00f;//速度消散速度
		float curlIntensity = 80.f;//涡流强度
		float splatRadius = 0.25f;//施加颜色的半径
		float splatForce = 6000.f;//施加速度的强度
		const unsigned int pressureIteraion = 50;//雅可比迭代次数 这个值越高物理模拟越不容易出错 NVIDIA的文章有提到通常20-50次就够了
		const unsigned int resolutionFactor = 2;//物理模拟分辨率
	}config;

	struct SimulationParam
	{
		DirectX::XMFLOAT2 pos = { 0,0 };
		DirectX::XMFLOAT2 posDelta = { 0,0 };
		DirectX::XMFLOAT4 splatColor = { 0,0,0,1 };
		DirectX::XMFLOAT2 colorTexelSize;
		DirectX::XMFLOAT2 simTexelSize;
		DirectX::XMUINT2 colorTextureSize;
		DirectX::XMUINT2 simTextureSize;
		float colorDissipationSpeed;
		float velocityDissipationSpeed;
		float curlIntensity;
		float splatRadius;
		DirectX::XMFLOAT4 padding[11];
	} simulationParam;

	ConstantBuffer* simulationParamBuffer;

	Timer colorUpdateTimer;

	Shader* splatVelocityCS;

	ComPtr<ID3D12PipelineState> splatVelocityState;

	Shader* splatColorCS;

	ComPtr<ID3D12PipelineState> splatColorState;

	Shader* divergenceCS;

	ComPtr<ID3D12PipelineState> divergenceState;

	Shader* pressureResetCS;

	ComPtr<ID3D12PipelineState> pressureResetState;

	Shader* pressureCS;

	ComPtr<ID3D12PipelineState> pressureState;

	Shader* gradientSubtractCS;

	ComPtr<ID3D12PipelineState> gradientSubtractState;

	Shader* velocityAdvectionCS;

	ComPtr<ID3D12PipelineState> velocityAdvectionState;

	Shader* colorAdvectionCS;

	ComPtr<ID3D12PipelineState> colorAdvectionState;

	Shader* fluidFinalPS;

	ComPtr<ID3D12PipelineState> fluidFinalState;

	ComPtr<ID3D12PipelineState> fullScreenState;

};