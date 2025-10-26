#pragma once

#include<Gear/Core/RenderTask.h>
#include<Gear/Core/Shader.h>

#include<Gear/Core/Resource/TextureRenderView.h>

class MyRenderTask :public RenderTask
{
public:

	MyRenderTask()
	{
		pixelShader = new Core::Shader(Utils::File::getRootFolder() + L"PixelShader.cso");

		{
			D3D12_GRAPHICS_PIPELINE_STATE_DESC desc = Core::PipelineState::getDefaultFullScreenState();
			desc.NumRenderTargets = 1;
			desc.RTVFormats[0] = Core::Graphics::backBufferFormat;
			desc.PS = pixelShader->getByteCode();

			Core::GraphicsDevice::get()->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(&pipelineState));
		}
	}

	~MyRenderTask()
	{
		delete pixelShader;
	}

protected:

	void recordCommand() override
	{
		context->setDefRenderTarget();

		context->setPipelineState(pipelineState.Get());

		context->setViewportSimple(Core::Graphics::getWidth(), Core::Graphics::getHeight());

		context->setTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		context->draw(3, 1, 0, 0);
	}

private:

	ComPtr<ID3D12PipelineState> pipelineState;

	Core::Shader* pixelShader;

};