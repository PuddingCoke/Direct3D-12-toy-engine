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

		myTexture = CreateTextureRenderTarget(TEXTURE_VIEW_CREATE_SRV, "est.png",
			DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_UNKNOWN, false);

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
			desc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
			desc.SampleDesc.Count = 1;

			GraphicsDevice::get()->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(&pipelineState));
		}
	}

	~MyRenderPass()
	{
		delete myTexture;
		delete vertexShader;
		delete pixelShader;
	}

protected:

	void recordCommand() override
	{
		setDefRenderTarget();

		setPipelineState(pipelineState.Get());

		setViewport(1920u, 1080u);

		setScissorRect(0.f, 0.f, 1920.f, 1080.f);

		setTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		draw(3, 1, 0, 0);
	}

private:

	TextureRenderTarget* myTexture;

	ComPtr<ID3D12PipelineState> pipelineState;

	Shader* vertexShader;

	Shader* pixelShader;
};