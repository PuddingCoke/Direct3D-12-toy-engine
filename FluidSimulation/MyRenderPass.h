#pragma once

#include<Gear/Core/RenderPass.h>

#include<Gear/Core/Effect/BloomEffect.h>

#include<Gear/Utils/Color.h>

#include<DirectXColors.h>

class MyRenderPass :public RenderPass
{
public:

	MyRenderPass() :
		effect(new BloomEffect(context, Graphics::getWidth(), Graphics::getHeight())),
		colorUpdateTimer(1.f),
		fluidFinalPS(new Shader(Utils::getRootFolder() + "FluidFinalPS.cso")),
		splatColorPS(new Shader(Utils::getRootFolder() + "SplatColorPS.cso")),
		splatVelocityPS(new Shader(Utils::getRootFolder() + "SplatVelocityPS.cso")),
		colorAdvectionDissipationPS(new Shader(Utils::getRootFolder() + "ColorAdvectionDissipation.cso")),
		velocityAdvectionDissipationPS(new Shader(Utils::getRootFolder() + "VelocityAdvectionDissipation.cso")),
		pressureDissipationPS(new Shader(Utils::getRootFolder() + "PressureDissipationPS.cso")),
		curlPS(new Shader(Utils::getRootFolder() + "CurlPS.cso")),
		divergencePS(new Shader(Utils::getRootFolder() + "DivergencePS.cso")),
		pressureGradientSubtractPS(new Shader(Utils::getRootFolder() + "PressureGradientSubtractPS.cso")),
		viscousDiffusionPS(new Shader(Utils::getRootFolder() + "ViscousDiffusionPS.cso")),
		vorticityPS(new Shader(Utils::getRootFolder() + "VorticityPS.cso"))
	{
		effect->setThreshold(0.f);

		const DirectX::XMUINT2 simRes =
		{
			Graphics::getWidth() / config.resolutionFactor,
			Graphics::getHeight() / config.resolutionFactor
		};

		colorTex = new SwapTexture([=] {return new TextureRenderTarget(Graphics::getWidth(), Graphics::getHeight(), DXGI_FORMAT_R16G16B16A16_FLOAT, 1, 1, false, true,
			DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_R16G16B16A16_FLOAT); });

		velocityTex = new SwapTexture([=] {return new TextureRenderTarget(simRes.x, simRes.y, DXGI_FORMAT_R32G32_FLOAT, 1, 1, false, true,
			DXGI_FORMAT_R32G32_FLOAT, DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_R32G32_FLOAT); });

		pressureTex = new SwapTexture([=] {return new TextureRenderTarget(simRes.x, simRes.y, DXGI_FORMAT_R32_FLOAT, 1, 1, false, true,
			DXGI_FORMAT_R32_FLOAT, DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_R32_FLOAT); });

		curlTex = new TextureRenderTarget(simRes.x, simRes.y, DXGI_FORMAT_R32_FLOAT, 1, 1, false, true,
			DXGI_FORMAT_R32_FLOAT, DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_R32_FLOAT);

		divergenceTex = new TextureRenderTarget(simRes.x, simRes.y, DXGI_FORMAT_R32_FLOAT, 1, 1, false, true,
			DXGI_FORMAT_R32_FLOAT, DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_R32_FLOAT);

		originTexture = new TextureRenderTarget(Graphics::getWidth(), Graphics::getHeight(), DXGI_FORMAT_R16G16B16A16_FLOAT, 1, 1, false, true,
			DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_R16G16B16A16_FLOAT);

		simulationParamBuffer = new ConstantBuffer(sizeof(SimulationParam), true, nullptr, nullptr, nullptr);

		simulationParam.simTexelSize = DirectX::XMFLOAT2(1.f / simRes.x, 1.f / simRes.y);
		simulationParam.colorDissipationSpeed = config.colorDissipationSpeed;
		simulationParam.velocityDissipationSpeed = config.velocityDissipationSpeed;
		simulationParam.pressureDissipationSpeed = config.pressureDissipationSpeed;
		simulationParam.curlIntensity = config.curlIntensity;
		simulationParam.aspectRatio = Graphics::getAspectRatio();
		simulationParam.splatRadius = config.splatRadius / 100.f * Graphics::getAspectRatio();

		{
			D3D12_GRAPHICS_PIPELINE_STATE_DESC desc = getDefaultGraphicsPipelineDesc();
			desc.PS = fluidFinalPS->getByteCode();
			desc.RTVFormats[0] = originTexture->getTexture()->getFormat();

			GraphicsDevice::get()->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(&fluidFinalState));
		}

		{
			D3D12_GRAPHICS_PIPELINE_STATE_DESC desc = getDefaultGraphicsPipelineDesc();
			desc.PS = splatColorPS->getByteCode();
			desc.RTVFormats[0] = colorTex->read()->getTexture()->getFormat();

			GraphicsDevice::get()->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(&splatColorState));
		}

		{
			D3D12_GRAPHICS_PIPELINE_STATE_DESC desc = getDefaultGraphicsPipelineDesc();
			desc.PS = splatVelocityPS->getByteCode();
			desc.RTVFormats[0] = velocityTex->read()->getTexture()->getFormat();

			GraphicsDevice::get()->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(&splatVelocityState));
		}

		{
			D3D12_GRAPHICS_PIPELINE_STATE_DESC desc = getDefaultGraphicsPipelineDesc();
			desc.PS = colorAdvectionDissipationPS->getByteCode();
			desc.RTVFormats[0] = colorTex->read()->getTexture()->getFormat();

			GraphicsDevice::get()->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(&colorAdvectionDissipationState));
		}

		{
			D3D12_GRAPHICS_PIPELINE_STATE_DESC desc = getDefaultGraphicsPipelineDesc();
			desc.PS = velocityAdvectionDissipationPS->getByteCode();
			desc.RTVFormats[0] = velocityTex->read()->getTexture()->getFormat();

			GraphicsDevice::get()->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(&velocityAdvectionDissipationState));
		}

		{
			D3D12_GRAPHICS_PIPELINE_STATE_DESC desc = getDefaultGraphicsPipelineDesc();
			desc.PS = pressureDissipationPS->getByteCode();
			desc.RTVFormats[0] = pressureTex->read()->getTexture()->getFormat();

			GraphicsDevice::get()->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(&pressureDissipationState));
		}

		{
			D3D12_GRAPHICS_PIPELINE_STATE_DESC desc = getDefaultGraphicsPipelineDesc();
			desc.PS = curlPS->getByteCode();
			desc.RTVFormats[0] = curlTex->getTexture()->getFormat();

			GraphicsDevice::get()->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(&curlState));
		}

		{
			D3D12_GRAPHICS_PIPELINE_STATE_DESC desc = getDefaultGraphicsPipelineDesc();
			desc.PS = divergencePS->getByteCode();
			desc.RTVFormats[0] = divergenceTex->getTexture()->getFormat();

			GraphicsDevice::get()->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(&divergenceState));
		}

		{
			D3D12_GRAPHICS_PIPELINE_STATE_DESC desc = getDefaultGraphicsPipelineDesc();
			desc.PS = pressureGradientSubtractPS->getByteCode();
			desc.RTVFormats[0] = velocityTex->read()->getTexture()->getFormat();

			GraphicsDevice::get()->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(&pressureGradientSubtractState));
		}

		{
			D3D12_GRAPHICS_PIPELINE_STATE_DESC desc = getDefaultGraphicsPipelineDesc();
			desc.PS = viscousDiffusionPS->getByteCode();
			desc.RTVFormats[0] = pressureTex->read()->getTexture()->getFormat();

			GraphicsDevice::get()->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(&viscousDiffusionState));
		}

		{
			D3D12_GRAPHICS_PIPELINE_STATE_DESC desc = getDefaultGraphicsPipelineDesc();
			desc.PS = vorticityPS->getByteCode();
			desc.RTVFormats[0] = velocityTex->read()->getTexture()->getFormat();

			GraphicsDevice::get()->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(&vorticityState));
		}

		{
			D3D12_GRAPHICS_PIPELINE_STATE_DESC desc = getDefaultGraphicsPipelineDesc();
			desc.PS = Shader::fullScreenPS->getByteCode();
			desc.RTVFormats[0] = Graphics::BackBufferFormat;

			GraphicsDevice::get()->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(&fullScreenState));
		}
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

		delete fluidFinalPS;
		delete splatColorPS;
		delete splatVelocityPS;
		delete colorAdvectionDissipationPS;
		delete velocityAdvectionDissipationPS;
		delete pressureDissipationPS;
		delete curlPS;
		delete divergencePS;
		delete pressureGradientSubtractPS;
		delete viscousDiffusionPS;
		delete vorticityPS;
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

		if (Mouse::getMoved() && Mouse::getLeftDown())
		{
			context->setViewport(velocityTex->width, velocityTex->height);
			context->setScissorRect(0, 0, velocityTex->width, velocityTex->height);
			context->setPipelineState(splatVelocityState.Get());
			context->setRenderTargets({ velocityTex->write()->getRTVMipHandle(0) }, {});
			context->setGraphicsConstants({ velocityTex->read()->getAllSRVIndex() }, 0);
			context->draw(3, 1, 0, 0);
			velocityTex->swap();

			context->setViewport(colorTex->width, colorTex->height);
			context->setScissorRect(0, 0, colorTex->width, colorTex->height);
			context->setPipelineState(splatColorState.Get());
			context->setRenderTargets({ colorTex->write()->getRTVMipHandle(0) }, {});
			context->setGraphicsConstants({ colorTex->read()->getAllSRVIndex() }, 0);
			context->draw(3, 1, 0, 0);
			colorTex->swap();
		}

		context->setViewport(velocityTex->width, velocityTex->height);
		context->setScissorRect(0, 0, velocityTex->width, velocityTex->height);

		context->setPipelineState(curlState.Get());
		context->setRenderTargets({ curlTex->getRTVMipHandle(0) }, {});
		context->setGraphicsConstants({ velocityTex->read()->getAllSRVIndex() }, 0);
		context->draw(3, 1, 0, 0);

		context->setPipelineState(vorticityState.Get());
		context->setRenderTargets({ velocityTex->write()->getRTVMipHandle(0) }, {});
		context->setGraphicsConstants({ velocityTex->read()->getAllSRVIndex(),curlTex->getAllSRVIndex() }, 0);
		context->draw(3, 1, 0, 0);
		velocityTex->swap();

		context->setPipelineState(divergenceState.Get());
		context->setRenderTargets({ divergenceTex->getRTVMipHandle(0) }, {});
		context->setGraphicsConstants({ velocityTex->read()->getAllSRVIndex() }, 0);
		context->draw(3, 1, 0, 0);

		context->setPipelineState(pressureDissipationState.Get());
		context->setRenderTargets({ pressureTex->write()->getRTVMipHandle(0) }, {});
		context->setGraphicsConstants({ pressureTex->read()->getAllSRVIndex() }, 0);
		context->draw(3, 1, 0, 0);
		pressureTex->swap();

		context->setPipelineState(viscousDiffusionState.Get());
		for (UINT i = 0; i < config.pressureIteraion; i++)
		{
			context->setRenderTargets({ pressureTex->write()->getRTVMipHandle(0) }, {});
			context->setGraphicsConstants({ pressureTex->read()->getAllSRVIndex(),divergenceTex->getAllSRVIndex() }, 0);
			context->draw(3, 1, 0, 0);
			pressureTex->swap();
		}

		context->setPipelineState(pressureGradientSubtractState.Get());
		context->setRenderTargets({ velocityTex->write()->getRTVMipHandle(0) }, {});
		context->setGraphicsConstants({ pressureTex->read()->getAllSRVIndex(),velocityTex->read()->getAllSRVIndex() }, 0);
		context->draw(3, 1, 0, 0);
		velocityTex->swap();

		context->setPipelineState(velocityAdvectionDissipationState.Get());
		context->setRenderTargets({ velocityTex->write()->getRTVMipHandle(0) }, {});
		context->setGraphicsConstants({ velocityTex->read()->getAllSRVIndex() }, 0);
		context->draw(3, 1, 0, 0);
		velocityTex->swap();

		context->setViewport(colorTex->width, colorTex->height);
		context->setScissorRect(0, 0, colorTex->width, colorTex->height);

		context->setPipelineState(colorAdvectionDissipationState.Get());
		context->setRenderTargets({ colorTex->write()->getRTVMipHandle(0) }, {});
		context->setGraphicsConstants({ velocityTex->read()->getAllSRVIndex(),colorTex->read()->getAllSRVIndex() }, 0);
		context->draw(3, 1, 0, 0);
		colorTex->swap();

		context->setPipelineState(fluidFinalState.Get());
		context->setRenderTargets({ originTexture->getRTVMipHandle(0) }, {});
		context->setGraphicsConstants({ colorTex->read()->getAllSRVIndex() }, 0);
		context->draw(3, 1, 0, 0);

		TextureRenderTarget* const outputTexture = effect->process(originTexture);

		context->setPipelineState(fullScreenState.Get());
		context->setDefRenderTarget();
		context->clearDefRenderTarget(DirectX::Colors::Black);
		context->setGraphicsConstants({ outputTexture->getAllSRVIndex() }, 0);
		context->draw(3, 1, 0, 0);
	}

private:

	BloomEffect* effect;

	SwapTexture* colorTex;

	SwapTexture* velocityTex;

	TextureRenderTarget* curlTex;

	TextureRenderTarget* divergenceTex;

	SwapTexture* pressureTex;

	TextureRenderTarget* originTexture;

	struct Config
	{
		float colorChangeSpeed = 10.f;//颜色改变速度
		float colorDissipationSpeed = 1.f;//颜色消散速度
		float velocityDissipationSpeed = 0.2f;//速度消散速度
		float pressureDissipationSpeed = 0.25f;//压力消散速度
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
		DirectX::XMFLOAT2 simTexelSize;
		float colorDissipationSpeed;
		float velocityDissipationSpeed;
		float pressureDissipationSpeed;
		float curlIntensity;
		float aspectRatio;
		float splatRadius;
		DirectX::XMFLOAT4 padding[12];
	} simulationParam;

	ConstantBuffer* simulationParamBuffer;

	Timer colorUpdateTimer;

	Shader* fluidFinalPS;

	ComPtr<ID3D12PipelineState> fluidFinalState;

	Shader* splatColorPS;

	ComPtr<ID3D12PipelineState> splatColorState;

	Shader* splatVelocityPS;

	ComPtr<ID3D12PipelineState> splatVelocityState;

	Shader* colorAdvectionDissipationPS;

	ComPtr<ID3D12PipelineState> colorAdvectionDissipationState;

	Shader* velocityAdvectionDissipationPS;

	ComPtr<ID3D12PipelineState> velocityAdvectionDissipationState;

	Shader* pressureDissipationPS;

	ComPtr<ID3D12PipelineState> pressureDissipationState;

	Shader* curlPS;

	ComPtr<ID3D12PipelineState> curlState;

	Shader* divergencePS;

	ComPtr<ID3D12PipelineState> divergenceState;

	Shader* pressureGradientSubtractPS;

	ComPtr<ID3D12PipelineState> pressureGradientSubtractState;

	Shader* viscousDiffusionPS;

	ComPtr<ID3D12PipelineState> viscousDiffusionState;

	Shader* vorticityPS;

	ComPtr<ID3D12PipelineState> vorticityState;

	ComPtr<ID3D12PipelineState> fullScreenState;
};