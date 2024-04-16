#pragma once

#include<Gear/Core/RenderPass.h>

#include<Gear/Core/Shader.h>

class MyRenderPass :public RenderPass
{
public:

	MyRenderPass() :
		textureCubePS(new Shader(Utils::getRootFolder() + "TextureCubePS.cso"))
	{
		begin();

		envCube = resManager->createTextureCube("ColdSunsetEquirect.png", 1024, true);

		randomGauss = resManager->createTextureRenderTarget(1024, 1024, RandomDataType::GAUSS, true);

		end();

		RenderEngine::get()->submitRenderPass(this);

		{
			D3D12_GRAPHICS_PIPELINE_STATE_DESC desc = {};
			desc.InputLayout = {};
			desc.pRootSignature = GlobalRootSignature::getGraphicsRootSignature()->get();
			desc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
			desc.RasterizerState = States::rasterCullNone;
			desc.DepthStencilState.DepthEnable = false;
			desc.DepthStencilState.StencilEnable = false;
			desc.SampleMask = UINT_MAX;
			desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
			desc.NumRenderTargets = 1;
			desc.RTVFormats[0] = Graphics::BackBufferFormat;
			desc.SampleDesc.Count = 1;
			desc.VS = Shader::textureCubeVS->getByteCode();
			desc.PS = textureCubePS->getByteCode();

			GraphicsDevice::get()->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(&skyboxState));
		}
	}

	~MyRenderPass()
	{
		delete textureCubePS;

		delete envCube;

		delete randomGauss;
	}

protected:

	void recordCommand() override
	{
		context->setPipelineState(skyboxState.Get());
		context->setViewport(Graphics::getWidth(), Graphics::getHeight());
		context->setScissorRect(0, 0, Graphics::getWidth(), Graphics::getHeight());
		context->setTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		context->setDefRenderTarget();

		context->setPSConstants({ envCube->getAllSRVIndex() }, 0);

		context->transitionResources();

		context->draw(36, 1, 0, 0);
	}

private:

	Shader* textureCubePS;

	ComPtr<ID3D12PipelineState> skyboxState;

	TextureRenderTarget* envCube;

	TextureRenderTarget* randomGauss;

};