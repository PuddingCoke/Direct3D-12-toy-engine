#include<Gear/Core/GlobalRootSignature.h>

GlobalRootSignature* GlobalRootSignature::instance = nullptr;

RootSignature* GlobalRootSignature::getGraphicsRootSignature()
{
	return instance->graphicsRootSignature;
}

RootSignature* GlobalRootSignature::getComputeRootSignature()
{
	return instance->computeRootSignature;
}

GlobalRootSignature::GlobalRootSignature()
{
	CD3DX12_STATIC_SAMPLER_DESC samplerDesc[7] = {};

	for (UINT i = 0; i < 7; i++)
	{
		samplerDesc[i].ShaderRegister = i;
		samplerDesc[i].RegisterSpace = 0;
		samplerDesc[i].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	}

	{
		samplerDesc[0].Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;
		samplerDesc[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		samplerDesc[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		samplerDesc[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		samplerDesc[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
		samplerDesc[0].MinLOD = 0.0;
		samplerDesc[0].MaxLOD = D3D12_FLOAT32_MAX;
	}

	{
		samplerDesc[1].Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;
		samplerDesc[1].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		samplerDesc[1].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		samplerDesc[1].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		samplerDesc[1].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
		samplerDesc[1].MinLOD = 0;
		samplerDesc[1].MaxLOD = D3D12_FLOAT32_MAX;
	}

	{
		samplerDesc[2].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
		samplerDesc[2].AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		samplerDesc[2].AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		samplerDesc[2].AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		samplerDesc[2].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
		samplerDesc[2].MinLOD = 0;
		samplerDesc[2].MaxLOD = D3D12_FLOAT32_MAX;
	}

	{
		samplerDesc[3].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
		samplerDesc[3].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		samplerDesc[3].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		samplerDesc[3].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		samplerDesc[3].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
		samplerDesc[3].MinLOD = 0;
		samplerDesc[3].MaxLOD = D3D12_FLOAT32_MAX;
	}

	{
		samplerDesc[4].Filter = D3D12_FILTER_ANISOTROPIC;
		samplerDesc[4].AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		samplerDesc[4].AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		samplerDesc[4].AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		samplerDesc[4].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
		samplerDesc[4].MaxAnisotropy = 16;
		samplerDesc[4].MinLOD = 0.f;
		samplerDesc[4].MaxLOD = D3D12_FLOAT32_MAX;
	}

	{
		samplerDesc[5].Filter = D3D12_FILTER_ANISOTROPIC;
		samplerDesc[5].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		samplerDesc[5].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		samplerDesc[5].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		samplerDesc[5].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
		samplerDesc[5].MaxAnisotropy = 16;
		samplerDesc[5].MinLOD = 0.f;
		samplerDesc[5].MaxLOD = D3D12_FLOAT32_MAX;
	}

	{
		samplerDesc[6].Filter = D3D12_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
		samplerDesc[6].AddressU = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
		samplerDesc[6].AddressV = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
		samplerDesc[6].AddressW = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
		samplerDesc[6].BorderColor = D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK;
		samplerDesc[6].ComparisonFunc = D3D12_COMPARISON_FUNC_GREATER;
		samplerDesc[6].MinLOD = 0.f;
		samplerDesc[6].MaxLOD = D3D12_FLOAT32_MAX;
	}

	{
		CD3DX12_ROOT_PARAMETER1 rootParameters[10] = {};

		//global constant buffer that can be accessed by any shader stage at any time
		rootParameters[0].InitAsConstantBufferView(0, 0, D3D12_ROOT_DESCRIPTOR_FLAG_NONE, D3D12_SHADER_VISIBILITY_ALL);
		//global constants
		rootParameters[1].InitAsConstants(16, 1, 0, D3D12_SHADER_VISIBILITY_ALL);
		//user defined global constant buffer that can be accessed by any shader stage within one frame
		rootParameters[2].InitAsConstantBufferView(2, 0, D3D12_ROOT_DESCRIPTOR_FLAG_NONE, D3D12_SHADER_VISIBILITY_ALL);
		//user defined per stage per draw call constant buffer
		rootParameters[3].InitAsConstantBufferView(3, 0, D3D12_ROOT_DESCRIPTOR_FLAG_NONE, D3D12_SHADER_VISIBILITY_AMPLIFICATION);
		rootParameters[4].InitAsConstantBufferView(3, 0, D3D12_ROOT_DESCRIPTOR_FLAG_NONE, D3D12_SHADER_VISIBILITY_DOMAIN);
		rootParameters[5].InitAsConstantBufferView(3, 0, D3D12_ROOT_DESCRIPTOR_FLAG_NONE, D3D12_SHADER_VISIBILITY_GEOMETRY);
		rootParameters[6].InitAsConstantBufferView(3, 0, D3D12_ROOT_DESCRIPTOR_FLAG_NONE, D3D12_SHADER_VISIBILITY_HULL);
		rootParameters[7].InitAsConstantBufferView(3, 0, D3D12_ROOT_DESCRIPTOR_FLAG_NONE, D3D12_SHADER_VISIBILITY_MESH);
		rootParameters[8].InitAsConstantBufferView(3, 0, D3D12_ROOT_DESCRIPTOR_FLAG_NONE, D3D12_SHADER_VISIBILITY_PIXEL);
		rootParameters[9].InitAsConstantBufferView(3, 0, D3D12_ROOT_DESCRIPTOR_FLAG_NONE, D3D12_SHADER_VISIBILITY_VERTEX);

		CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
		rootSignatureDesc.Init_1_1(_countof(rootParameters), rootParameters, _countof(samplerDesc), samplerDesc, 
			D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT | D3D12_ROOT_SIGNATURE_FLAG_CBV_SRV_UAV_HEAP_DIRECTLY_INDEXED | D3D12_ROOT_SIGNATURE_FLAG_SAMPLER_HEAP_DIRECTLY_INDEXED);

		graphicsRootSignature = new RootSignature(rootSignatureDesc);
	}

	{
		CD3DX12_ROOT_PARAMETER1 rootParameters[4] = {};
		//global constant buffer that can be accessed by any shader stage at any time
		rootParameters[0].InitAsConstantBufferView(0, 0, D3D12_ROOT_DESCRIPTOR_FLAG_NONE, D3D12_SHADER_VISIBILITY_ALL);
		//global constants
		rootParameters[1].InitAsConstants(16, 1, 0, D3D12_SHADER_VISIBILITY_ALL);
		//user defined global constant buffer that can be accessed by any shader stage within one frame
		rootParameters[2].InitAsConstantBufferView(2, 0, D3D12_ROOT_DESCRIPTOR_FLAG_NONE, D3D12_SHADER_VISIBILITY_ALL);
		//user defined per stage per draw call constant buffer
		rootParameters[3].InitAsConstantBufferView(3, 0, D3D12_ROOT_DESCRIPTOR_FLAG_NONE, D3D12_SHADER_VISIBILITY_ALL);

		CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
		rootSignatureDesc.Init_1_1(_countof(rootParameters), rootParameters, _countof(samplerDesc), samplerDesc, 
			D3D12_ROOT_SIGNATURE_FLAG_CBV_SRV_UAV_HEAP_DIRECTLY_INDEXED | D3D12_ROOT_SIGNATURE_FLAG_SAMPLER_HEAP_DIRECTLY_INDEXED);

		computeRootSignature = new RootSignature(rootSignatureDesc);
	}
}

GlobalRootSignature::~GlobalRootSignature()
{
	if (graphicsRootSignature)
	{
		delete graphicsRootSignature;
	}

	if (computeRootSignature)
	{
		delete computeRootSignature;
	}
}
