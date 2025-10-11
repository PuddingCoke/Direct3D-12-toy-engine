#pragma once

#include<Gear/Core/RenderTask.h>
#include<Gear/Core/Shader.h>

#include<Gear/Core/Resource/TextureRenderView.h>

class MyRenderTask :public RenderTask
{
public:

	MyRenderTask()
	{
		vertexShader = new Shader(Utils::getRootFolder() + L"VertexShader.cso");

		pixelShader = new Shader(Utils::getRootFolder() + L"PixelShader.cso");

		{
			D3D12_GRAPHICS_PIPELINE_STATE_DESC desc = PipelineState::getDefaultGraphicsDesc();
			desc.InputLayout = {};
			desc.VS = vertexShader->getByteCode();
			desc.PS = pixelShader->getByteCode();
			desc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
			desc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
			desc.DepthStencilState.DepthEnable = FALSE;
			desc.DepthStencilState.StencilEnable = FALSE;
			desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
			desc.NumRenderTargets = 1;
			desc.RTVFormats[0] = Graphics::backBufferFormat;

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

		context->setViewportSimple(Graphics::getWidth(), Graphics::getHeight());

		context->setTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		context->draw(3, 1, 0, 0);
	}

private:

	ComPtr<ID3D12PipelineState> pipelineState;

	Shader* vertexShader;

	Shader* pixelShader;
};