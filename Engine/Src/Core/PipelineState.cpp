#include<Gear/Core/PipelineState.h>

#include<Gear/Core/Shader.h>

#include<Gear/Core/GlobalRootSignature.h>

D3D12_BLEND_DESC PipelineState::defBlendDesc = {};

D3D12_BLEND_DESC PipelineState::addtiveBlendDesc = {};

D3D12_RASTERIZER_DESC PipelineState::rasterShadow = {};

D3D12_RASTERIZER_DESC PipelineState::rasterCullBack = {};

D3D12_RASTERIZER_DESC PipelineState::rasterCullFront = {};

D3D12_RASTERIZER_DESC PipelineState::rasterCullNone = {};

D3D12_RASTERIZER_DESC PipelineState::rasterWireFrame = {};

D3D12_DEPTH_STENCIL_DESC PipelineState::depthLessEqual = {};

D3D12_DEPTH_STENCIL_DESC PipelineState::depthLess = {};

D3D12_GRAPHICS_PIPELINE_STATE_DESC PipelineState::getDefaultGraphicsDesc()
{
	D3D12_GRAPHICS_PIPELINE_STATE_DESC desc = {};
	desc.pRootSignature = GlobalRootSignature::getGraphicsRootSignature()->get();
	desc.SampleMask = UINT_MAX;
	desc.SampleDesc.Count = 1;

	return desc;
}

D3D12_GRAPHICS_PIPELINE_STATE_DESC PipelineState::getDefaultFullScreenState()
{
	D3D12_GRAPHICS_PIPELINE_STATE_DESC desc = {};
	desc.InputLayout = {};
	desc.pRootSignature = GlobalRootSignature::getGraphicsRootSignature()->get();
	desc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	desc.RasterizerState = rasterCullNone;
	desc.DepthStencilState.DepthEnable = false;
	desc.DepthStencilState.StencilEnable = false;
	desc.SampleMask = UINT_MAX;
	desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	desc.SampleDesc.Count = 1;
	desc.VS = Shader::fullScreenVS->getByteCode();

	return desc;
}

void PipelineState::createComputeState(ID3D12PipelineState** const pipelineState, const Shader* const shader)
{
	D3D12_COMPUTE_PIPELINE_STATE_DESC desc = {};
	desc.pRootSignature = GlobalRootSignature::getComputeRootSignature()->get();
	desc.CS = shader->getByteCode();

	GraphicsDevice::get()->CreateComputePipelineState(&desc, IID_PPV_ARGS(pipelineState));
}

void PipelineState::initialize()
{
	{
		defBlendDesc.IndependentBlendEnable = false;

		defBlendDesc.RenderTarget[0].BlendEnable = true;
		defBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

		defBlendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
		defBlendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;

		defBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
		defBlendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_INV_SRC_ALPHA;

		defBlendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
		defBlendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	}

	{
		addtiveBlendDesc.IndependentBlendEnable = false;

		addtiveBlendDesc.RenderTarget[0].BlendEnable = true;
		addtiveBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

		addtiveBlendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_ONE;
		addtiveBlendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;

		addtiveBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
		addtiveBlendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;

		addtiveBlendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
		addtiveBlendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	}

	{
		rasterShadow.FillMode = D3D12_FILL_MODE_SOLID;
		rasterShadow.CullMode = D3D12_CULL_MODE_BACK;
		rasterShadow.DepthClipEnable = TRUE;
		rasterShadow.DepthBias = 16;
		rasterShadow.SlopeScaledDepthBias = 4.f;
	}

	{
		rasterCullBack.FillMode = D3D12_FILL_MODE_SOLID;
		rasterCullBack.CullMode = D3D12_CULL_MODE_BACK;
		rasterCullBack.DepthClipEnable = TRUE;
	}

	{
		rasterCullFront.FillMode = D3D12_FILL_MODE_SOLID;
		rasterCullFront.CullMode = D3D12_CULL_MODE_FRONT;
		rasterCullFront.DepthClipEnable = TRUE;
	}

	{
		rasterCullNone.FillMode = D3D12_FILL_MODE_SOLID;
		rasterCullNone.CullMode = D3D12_CULL_MODE_NONE;
		rasterCullNone.DepthClipEnable = TRUE;
	}

	{
		rasterWireFrame.FillMode = D3D12_FILL_MODE_WIREFRAME;
		rasterWireFrame.CullMode = D3D12_CULL_MODE_BACK;
		rasterWireFrame.DepthClipEnable = TRUE;
	}

	{
		depthLessEqual.DepthEnable = true;
		depthLessEqual.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
		depthLessEqual.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
		depthLessEqual.StencilEnable = false;
	}

	{
		depthLess.DepthEnable = true;
		depthLess.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
		depthLess.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
		depthLess.StencilEnable = false;
	}
}
