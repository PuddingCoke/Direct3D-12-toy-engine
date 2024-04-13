#pragma once

#include<Gear/Core/RenderPass.h>

#include<Gear/Core/Shader.h>

#include<Gear/Core/Resource/TextureRenderTarget.h>

#include<Gear/Utils/Math.h>
#include<Gear/Utils/Random.h>

class MyRenderPass :public RenderPass
{
public:

	MyRenderPass() :
		accumulateShader(new Shader(Utils::getRootFolder() + "AccumulateShader.cso")),
		displayShader(new Shader(Utils::getRootFolder() + "DisplayShader.cso")),
		accumulatedTexture(ResourceManager::createTextureRenderTarget(Graphics::getWidth(), Graphics::getHeight(), DXGI_FORMAT_R16G16B16A16_UNORM, 1, 1, false, false,
			DXGI_FORMAT_R16G16B16A16_UNORM, DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_R16G16B16A16_UNORM)),
		cameraParam{ Math::pi / 4.f + 0.4f,0.f,3.0f,8.f },
		accumulateParam{ 0,0.f }
	{
		{
			D3D12_GRAPHICS_PIPELINE_STATE_DESC desc = {};
			desc.InputLayout = {};
			desc.pRootSignature = GlobalRootSignature::getGraphicsRootSignature()->get();
			desc.VS = Shader::fullScreenVS->getByteCode();
			desc.PS = accumulateShader->getByteCode();
			desc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
			desc.BlendState = States::defBlendDesc;
			desc.DepthStencilState.DepthEnable = FALSE;
			desc.DepthStencilState.StencilEnable = FALSE;
			desc.SampleMask = UINT_MAX;
			desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
			desc.NumRenderTargets = 1;
			desc.RTVFormats[0] = accumulatedTexture->getTexture()->getFormat();
			desc.SampleDesc.Count = 1;

			GraphicsDevice::get()->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(&accumulateState));
		}

		{
			D3D12_GRAPHICS_PIPELINE_STATE_DESC desc = {};
			desc.InputLayout = {};
			desc.pRootSignature = GlobalRootSignature::getGraphicsRootSignature()->get();
			desc.VS = Shader::fullScreenVS->getByteCode();
			desc.PS = displayShader->getByteCode();
			desc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
			desc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
			desc.DepthStencilState.DepthEnable = FALSE;
			desc.DepthStencilState.StencilEnable = FALSE;
			desc.SampleMask = UINT_MAX;
			desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
			desc.NumRenderTargets = 1;
			desc.RTVFormats[0] = Graphics::BackBufferFormat;
			desc.SampleDesc.Count = 1;

			GraphicsDevice::get()->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(&displayState));
		}

		Mouse::addMoveEvent([this]()
			{
				if (Mouse::getLeftDown())
				{
					cameraParam.phi -= Mouse::getDY() * Graphics::getDeltaTime();
					cameraParam.theta += Mouse::getDX() * Graphics::getDeltaTime();
					cameraParam.phi = Math::clamp(cameraParam.phi, -Math::half_pi + 0.01f, Math::half_pi - 0.01f);

					accumulateParam.frameIndex = 0;
				}
			});

		Mouse::addScrollEvent([this]()
			{
				cameraParam.radius -= Mouse::getWheelDelta() * 1.f;

				accumulateParam.frameIndex = 0;
			});

	}

	~MyRenderPass()
	{
		delete accumulateShader;
		delete displayShader;
		delete accumulatedTexture;
	}

protected:

	void recordCommand() override
	{
		accumulateParam.frameIndex++;

		accumulateParam.floatSeed = Graphics::getTimeElapsed();

		accumulatedTexture->copyDescriptors();

		context->setTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		context->setViewport(Graphics::getWidth(), Graphics::getHeight());

		context->setScissorRect(0.f, 0.f, static_cast<float>(Graphics::getWidth()), static_cast<float>(Graphics::getHeight()));

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

	TextureRenderTarget* accumulatedTexture;

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