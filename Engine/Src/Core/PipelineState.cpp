#include<Gear/Core/PipelineState.h>

#include<Gear/Core/GlobalRootSignature.h>

D3D12_GRAPHICS_PIPELINE_STATE_DESC Core::PipelineState::getDefaultGraphicsDesc()
{
	D3D12_GRAPHICS_PIPELINE_STATE_DESC desc = {};
	desc.pRootSignature = Core::GlobalRootSignature::getGraphicsRootSignature()->get();
	desc.SampleMask = UINT_MAX;
	desc.SampleDesc.Count = 1;

	return desc;
}

D3D12_GRAPHICS_PIPELINE_STATE_DESC Core::PipelineState::getDefaultFullScreenState()
{
	D3D12_GRAPHICS_PIPELINE_STATE_DESC desc = {};
	desc.InputLayout = {};
	desc.pRootSignature = Core::GlobalRootSignature::getGraphicsRootSignature()->get();
	desc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	desc.RasterizerState = rasterCullNone;
	desc.DepthStencilState.DepthEnable = false;
	desc.DepthStencilState.StencilEnable = false;
	desc.SampleMask = UINT_MAX;
	desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	desc.SampleDesc.Count = 1;
	desc.VS = Core::GlobalShader::getFullScreenVS()->getByteCode();

	return desc;
}

void Core::PipelineState::createComputeState(ID3D12PipelineState** const pipelineState, const Shader* const shader)
{
	D3D12_COMPUTE_PIPELINE_STATE_DESC desc = {};
	desc.pRootSignature = Core::GlobalRootSignature::getComputeRootSignature()->get();
	desc.CS = shader->getByteCode();

	Core::GraphicsDevice::get()->CreateComputePipelineState(&desc, IID_PPV_ARGS(pipelineState));
}
