#pragma once

#include<Gear/Core/RenderPass.h>
#include<Gear/Core/Shader.h>

#include<Gear/Core/Resource/TextureRenderTarget.h>

class MyRenderPass :public RenderPass
{
public:

	MyRenderPass()
	{
		begin();

		envCube = resManager->createTextureCube("ColdSunsetEquirect.png", 1024, true);

		nightENVCube = resManager->createTextureCube(
			{
			"SkyEarlyDusk_Right.png",
			"SkyEarlyDusk_Left.png",
			"SkyEarlyDusk_Top.png",
			"SkyEarlyDusk_Bottom.png",
			"SkyEarlyDusk_Front.png",
			"SkyEarlyDusk_Back.png"
			},
			true);

		randomNoise = resManager->createTextureRenderTarget(1920, 1080, RandomDataType::GAUSS, true);

		end();

		RenderEngine::get()->submitRenderPass(this);

		vertexShader = new Shader(Utils::getRootFolder() + "VertexShader.cso");

		pixelShader = new Shader(Utils::getRootFolder() + "PixelShader.cso");

		{
			D3D12_GRAPHICS_PIPELINE_STATE_DESC desc = {};
			desc.InputLayout = {};
			desc.pRootSignature = GlobalRootSignature::getGraphicsRootSignature()->get();
			desc.VS = vertexShader->getByteCode();
			desc.PS = pixelShader->getByteCode();
			desc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
			desc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
			desc.DepthStencilState.DepthEnable = FALSE;
			desc.DepthStencilState.StencilEnable = FALSE;
			desc.SampleMask = UINT_MAX;
			desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
			desc.NumRenderTargets = 1;
			desc.RTVFormats[0] = Graphics::BackBufferFormat;
			desc.SampleDesc.Count = 1;

			GraphicsDevice::get()->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(&pipelineState));
		}
	}

	~MyRenderPass()
	{
		delete vertexShader;
		delete pixelShader;
		delete envCube;
		delete nightENVCube;
		delete randomNoise;
	}

protected:

	void recordCommand() override
	{
		context->setDefRenderTarget();

		context->setPipelineState(pipelineState.Get());

		context->setViewport(1920u, 1080u);

		context->setScissorRect(0.f, 0.f, 1920.f, 1080.f);

		context->setTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		context->draw(3, 1, 0, 0);
	}

private:

	ComPtr<ID3D12PipelineState> pipelineState;

	TextureRenderTarget* envCube;

	TextureRenderTarget* nightENVCube;

	TextureRenderTarget* randomNoise;

	Shader* vertexShader;

	Shader* pixelShader;
};