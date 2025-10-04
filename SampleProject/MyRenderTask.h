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

		LOGUSER(L"示例项目", L"Sample Project", L"Пример проекта");

		LOGUSER(L"32位有符号整数测试", IntegerMode::HEX, 12895, -123456, INT_MAX, INT_MIN, IntegerMode::DEC, 12895, -123456, INT_MAX, INT_MIN);

		LOGUSER(L"32位无符号整数测试", IntegerMode::HEX, 13689u, UINT_MAX, IntegerMode::DEC, 13689u, UINT_MAX);

		LOGUSER(L"64位有符号整数测试", IntegerMode::HEX, 1222222ll, -188888ll, INT64_MAX, INT64_MIN, IntegerMode::DEC, 1222222ll, -188888ll, INT64_MAX, INT64_MIN);

		LOGUSER(L"64位无符号整数测试", IntegerMode::HEX, 13579ull, UINT64_MAX, IntegerMode::DEC, 13579ull, UINT64_MAX);

		LOGUSER(L"浮点测试", FloatPrecision(4), 125.6f, FLT_MAX, FloatPrecision(2), 125.7, FLT_MAX);
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