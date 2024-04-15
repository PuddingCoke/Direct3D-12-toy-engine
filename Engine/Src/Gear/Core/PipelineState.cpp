#include<Gear/Core/PipelineState.h>

PipelineState* PipelineState::instance = nullptr;

PipelineState* PipelineState::get()
{
	return instance;
}

PipelineState::PipelineState()
{
	{
		Shader equirectangularVS = Shader(g_EquirectangularVSBytes, sizeof(g_EquirectangularVSBytes));

		Shader equirectangularPS = Shader(g_EquirectangularPSBytes, sizeof(g_EquirectangularPSBytes));

		D3D12_GRAPHICS_PIPELINE_STATE_DESC desc = {};
		desc.InputLayout = {};
		desc.pRootSignature = GlobalRootSignature::getGraphicsRootSignature()->get();
		desc.VS = equirectangularVS.getByteCode();
		desc.PS = equirectangularPS.getByteCode();
		desc.RasterizerState = States::rasterCullNone;
		desc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
		desc.DepthStencilState.DepthEnable = FALSE;
		desc.DepthStencilState.StencilEnable = FALSE;
		desc.SampleMask = UINT_MAX;
		desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		desc.NumRenderTargets = 1;
		desc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.SampleDesc.Count = 1;

		GraphicsDevice::get()->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(&equirectangularR8State));

		std::cout << "[class PipelineState] create equirectangularR8State\n";

		desc.RTVFormats[0] = DXGI_FORMAT_R16G16B16A16_FLOAT;

		GraphicsDevice::get()->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(&equirectangularR16State));

		std::cout << "[class PipelineState] create equirectangularR16State\n";
	}
}

PipelineState::~PipelineState()
{
}
