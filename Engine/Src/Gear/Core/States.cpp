#include<Gear/Core/States.h>

D3D12_BLEND_DESC States::defBlendDesc = {};

D3D12_BLEND_DESC States::addtiveBlendDesc = {};

D3D12_RASTERIZER_DESC States::rasterShadow = {};

D3D12_RASTERIZER_DESC States::rasterCullBack = {};

D3D12_RASTERIZER_DESC States::rasterCullFront = {};

D3D12_RASTERIZER_DESC States::rasterCullNone = {};

D3D12_RASTERIZER_DESC States::rasterWireFrame = {};

D3D12_DEPTH_STENCIL_DESC States::depthLessEqual = {};

D3D12_DEPTH_STENCIL_DESC States::depthLess = {};

void States::initialize()
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
