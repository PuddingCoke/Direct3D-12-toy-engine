#pragma once

#include<Gear/Core/RenderTask.h>
#include<Gear/Core/Shader.h>

#include<Gear/Core/Resource/TextureRenderView.h>

class MyRenderTask :public RenderTask
{
public:

	MyRenderTask()
	{
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
			desc.RTVFormats[0] = Graphics::backBufferFormat;
			desc.SampleDesc.Count = 1;

			GraphicsDevice::get()->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(&pipelineState));
		}
	}

	~MyRenderTask()
	{
		delete vertexShader;
		delete pixelShader;
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

	Shader* vertexShader;

	Shader* pixelShader;
};