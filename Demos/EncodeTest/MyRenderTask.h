#pragma once

#include<Gear/Core/RenderTask.h>

#include<Gear/DevEssential.h>

class MyRenderTask :public RenderTask
{
public:

	MyRenderTask() :
		pixelShader(Shader::create(File::getWRootFolder() + L"PixelShader.cso"))
	{
		state = PipelineStateBuilder()
			.setRasterizerState(PipelineStateHelper::rasterCullNone)
			.setDepthStencilState(PipelineStateHelper::depthCompareNone)
			.setBlendState(PipelineStateHelper::blendReplace)
			.setVS(*GlobalShader::getFullScreenVS())
			.setPS(*pixelShader)
			.build();
	}

	void recordCommand() override
	{
		context->setPipelineState(*state);

		context->setDefRenderTarget();

		context->setViewportSimple(Graphics::getWidth(), Graphics::getHeight());

		context->setPrimitiveTopology(TOPOLOGY::TRIANGLELIST);

		context->drawQuad();
	}

private:

	ShaderPtr pixelShader;

	GraphicsStatePtr state;

};
