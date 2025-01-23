#include<Gear/Core/PipelineState.h>

PipelineState* PipelineState::instance = nullptr;

PipelineState* PipelineState::get()
{
	return instance;
}

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
	desc.RasterizerState = States::rasterCullNone;
	desc.DepthStencilState.DepthEnable = false;
	desc.DepthStencilState.StencilEnable = false;
	desc.SampleMask = UINT_MAX;
	desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	desc.SampleDesc.Count = 1;
	desc.VS = Shader::fullScreenVS->getByteCode();

	return desc;
}

void PipelineState::createComputeState(ID3D12PipelineState** pipelineState, const Shader* const shader)
{
	D3D12_COMPUTE_PIPELINE_STATE_DESC desc = {};
	desc.pRootSignature = GlobalRootSignature::getComputeRootSignature()->get();
	desc.CS = shader->getByteCode();

	GraphicsDevice::get()->CreateComputePipelineState(&desc, IID_PPV_ARGS(pipelineState));
}

PipelineState::PipelineState()
{
	{
		D3D12_GRAPHICS_PIPELINE_STATE_DESC desc = getDefaultFullScreenState();
		desc.PS = Shader::fullScreenPS->getByteCode();
		desc.NumRenderTargets = 1;
		desc.RTVFormats[0] = Graphics::backBufferFormat;

		GraphicsDevice::get()->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(&fullScreenBlitState));

		LOGSUCCESS("create", Logger::brightMagenta, "fullScreenBlitState", Logger::resetColor(), "succeeded");
	}
}

PipelineState::~PipelineState()
{
}
