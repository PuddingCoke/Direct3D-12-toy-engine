#pragma once

#include<Gear/Core/RenderTask.h>

#include<Gear/Core/Shader.h>

#include<Gear/Core/Resource/TextureRenderView.h>

#include<Gear/Utils/Math.h>
#include<Gear/Utils/Random.h>

class MyRenderTask :public RenderTask
{
public:

	MyRenderTask() :
		accumulateShader(new Shader(Utils::File::getRootFolder() + L"AccumulateShader.cso")),
		displayShader(new Shader(Utils::File::getRootFolder() + L"DisplayShader.cso")),
		accumulatedTexture(ResourceManager::createTextureRenderView(Core::Graphics::getWidth(), Core::Graphics::getHeight(), DXGI_FORMAT_R16G16B16A16_UNORM, 1, 1, false, false,
			DXGI_FORMAT_R16G16B16A16_UNORM, DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_R16G16B16A16_UNORM)),
		cameraParam{ Utils::Math::pi / 4.f + 0.4f,0.f,3.0f,8.f },
		accumulateParam{ 0,0.f }
	{
		{
			D3D12_GRAPHICS_PIPELINE_STATE_DESC desc = Core::PipelineStateHelper::getDefaultGraphicsDesc();
			desc.InputLayout = {};
			desc.VS = Core::GlobalShader::getFullScreenVS()->getByteCode();
			desc.PS = accumulateShader->getByteCode();
			desc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
			desc.BlendState = Core::PipelineStateHelper::blendDefault;
			desc.DepthStencilState.DepthEnable = FALSE;
			desc.DepthStencilState.StencilEnable = FALSE;
			desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
			desc.NumRenderTargets = 1;
			desc.RTVFormats[0] = accumulatedTexture->getTexture()->getFormat();

			Core::GraphicsDevice::get()->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(&accumulateState));
		}

		{
			D3D12_GRAPHICS_PIPELINE_STATE_DESC desc = Core::PipelineStateHelper::getDefaultGraphicsDesc();
			desc.InputLayout = {};
			desc.VS = Core::GlobalShader::getFullScreenVS()->getByteCode();
			desc.PS = displayShader->getByteCode();
			desc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
			desc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
			desc.DepthStencilState.DepthEnable = FALSE;
			desc.DepthStencilState.StencilEnable = FALSE;
			desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
			desc.NumRenderTargets = 1;
			desc.RTVFormats[0] = Core::Graphics::backBufferFormat;

			Core::GraphicsDevice::get()->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(&displayState));
		}

		Input::Mouse::addMoveEvent([this]()
			{
				if (Input::Mouse::getLeftDown())
				{
					cameraParam.phi -= Input::Mouse::getDY() * Core::Graphics::getDeltaTime();
					cameraParam.theta += Input::Mouse::getDX() * Core::Graphics::getDeltaTime();
					cameraParam.phi = Utils::Math::clamp(cameraParam.phi, -Utils::Math::halfPi + 0.01f, Utils::Math::halfPi - 0.01f);

					accumulateParam.frameIndex = 0;
				}
			});

		Input::Mouse::addScrollEvent([this]()
			{
				cameraParam.radius -= Input::Mouse::getWheelDelta() * 1.f;

				accumulateParam.frameIndex = 0;
			});

	}

	void imGUICall() override
	{
		ImGui::Begin("Parameters");
		ImGui::SliderFloat("POWER", &cameraParam.POWER, 0.f, 12.f);
		ImGui::End();
	}

	~MyRenderTask()
	{
		delete accumulateShader;
		delete displayShader;
		delete accumulatedTexture;
	}

protected:

	void recordCommand() override
	{
		accumulateParam.frameIndex++;

		accumulateParam.floatSeed = Core::Graphics::getTimeElapsed();

		accumulatedTexture->copyDescriptors();

		context->setTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		context->setViewport(Core::Graphics::getWidth(), Core::Graphics::getHeight());

		context->setScissorRect(0.f, 0.f, static_cast<float>(Core::Graphics::getWidth()), static_cast<float>(Core::Graphics::getHeight()));

		context->setPipelineState(accumulateState.Get());

		context->setPSConstants(2, &accumulateParam, 0);

		context->setPSConstants(4, &cameraParam, 2);

		context->setRenderTargets({ accumulatedTexture->getRTVMipHandle(0) }, {});

		context->transitionResources();

		context->draw(3, 1, 0, 0);

		context->setPipelineState(displayState.Get());

		context->setPSConstants({ accumulatedTexture->getAllSRVIndex() }, 0);

		context->setDefRenderTarget();

		context->transitionResources();

		context->draw(3, 1, 0, 0);
	}

private:

	Shader* accumulateShader;

	Shader* displayShader;

	TextureRenderView* accumulatedTexture;

	ComPtr<ID3D12PipelineState> accumulateState;

	ComPtr<ID3D12PipelineState> displayState;

	struct CameraParam
	{
		float phi;
		float theta;
		float radius;
		float POWER;
	} cameraParam;

	struct AccumulateParam
	{
		UINT frameIndex;
		float floatSeed;
	} accumulateParam;

};