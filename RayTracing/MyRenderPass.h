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
		fullScreenVS(new Shader(Utils::getRootFolder() + "FullScreenVS.cso")),
		accumulateShader(new Shader(Utils::getRootFolder() + "AccumulateShader.cso")),
		displayShader(new Shader(Utils::getRootFolder() + "DisplayShader.cso")),
		accumulatedTexture(new TextureRenderTarget(TEXTURE_VIEW_CREATE_RTV | TEXTURE_VIEW_CREATE_SRV, 1920, 1080, DXGI_FORMAT_R16G16B16A16_UNORM,
			1, 1, DXGI_FORMAT_R16G16B16A16_UNORM, DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_R16G16B16A16_UNORM, false)),
		cameraParam{ 0.25f,0.0f,12.0f },
		accumulateParam{ 0,0.f }
	{
		{
			D3D12_BLEND_DESC blendDesc = {};
			blendDesc.AlphaToCoverageEnable = false;
			blendDesc.IndependentBlendEnable = false;
			blendDesc.RenderTarget[0].BlendEnable = true;
			blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
			blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
			blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
			blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
			blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_INV_SRC_ALPHA;
			blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
			blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;

			D3D12_GRAPHICS_PIPELINE_STATE_DESC desc = {};
			desc.InputLayout = {};
			desc.pRootSignature = GlobalRootSignature::getGraphicsRootSignature()->get();
			desc.VS = fullScreenVS->getByteCode();
			desc.PS = accumulateShader->getByteCode();
			desc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
			desc.BlendState = blendDesc;
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
			desc.VS = fullScreenVS->getByteCode();
			desc.PS = displayShader->getByteCode();
			desc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
			desc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
			desc.DepthStencilState.DepthEnable = FALSE;
			desc.DepthStencilState.StencilEnable = FALSE;
			desc.SampleMask = UINT_MAX;
			desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
			desc.NumRenderTargets = 1;
			desc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
			desc.SampleDesc.Count = 1;

			GraphicsDevice::get()->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(&displayState));
		}

		Mouse::addMoveEvent([this]()
			{
				if (Mouse::getLeftDown())
				{
					cameraParam.phi -= Mouse::getDY() * Graphics::getDeltaTime();
					cameraParam.theta += Mouse::getDX() * Graphics::getDeltaTime();
					cameraParam.phi = Math::clamp(cameraParam.phi, 0.01f, Math::half_pi - 0.01f);

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
		delete fullScreenVS;
		delete accumulateShader;
		delete displayShader;
		delete accumulatedTexture;
	}

protected:

	void recordCommand() override
	{
		accumulateParam.frameIndex++;

		accumulateParam.floatSeed = Graphics::getTimeElapsed();

		setTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		setViewport(1920u, 1080u);

		setScissorRect(0.f, 0.f, 1920.f, 1080.f);

		setPipelineState(accumulateState.Get());

		setGraphicsConstants(2, &accumulateParam, 0);

		setGraphicsConstants(3, &cameraParam, 2);

		setRenderTargets({ accumulatedTexture->getRTVMipHandle(0) }, {});

		draw(3, 1, 0, 0);

		setPipelineState(displayState.Get());

		setGraphicsConstants({ accumulatedTexture->getAllSRVIndex() }, 0);

		setDefRenderTarget();

		draw(3, 1, 0, 0);
	}

private:

	Shader* fullScreenVS;

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
	} cameraParam;

	struct AccumulateParam
	{
		UINT frameIndex;
		float floatSeed;
	} accumulateParam;

};