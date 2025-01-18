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

	for (uint32_t i = 0; i < 7; i++)
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
		//2+2+4+4+8+4+24+2*5 = 58 dwords
		CD3DX12_ROOT_PARAMETER1 rootParameters[12] = {};

		//reserved global constant buffer used by engine
		rootParameters[0].InitAsConstantBufferView(0, 0, D3D12_ROOT_DESCRIPTOR_FLAG_NONE, D3D12_SHADER_VISIBILITY_ALL);
		//user defined global per frame constant buffer
		rootParameters[1].InitAsConstantBufferView(1, 0, D3D12_ROOT_DESCRIPTOR_FLAG_NONE, D3D12_SHADER_VISIBILITY_ALL);
		//per draw call constants
		rootParameters[2].InitAsConstants(4, 2, 0, D3D12_SHADER_VISIBILITY_VERTEX);
		rootParameters[3].InitAsConstants(4, 2, 0, D3D12_SHADER_VISIBILITY_HULL);
		rootParameters[4].InitAsConstants(8, 2, 0, D3D12_SHADER_VISIBILITY_DOMAIN);
		rootParameters[5].InitAsConstants(4, 2, 0, D3D12_SHADER_VISIBILITY_GEOMETRY);
		rootParameters[6].InitAsConstants(24, 2, 0, D3D12_SHADER_VISIBILITY_PIXEL);
		//per draw call constant buffers
		rootParameters[7].InitAsConstantBufferView(3, 0, D3D12_ROOT_DESCRIPTOR_FLAG_NONE, D3D12_SHADER_VISIBILITY_VERTEX);
		rootParameters[8].InitAsConstantBufferView(3, 0, D3D12_ROOT_DESCRIPTOR_FLAG_NONE, D3D12_SHADER_VISIBILITY_HULL);
		rootParameters[9].InitAsConstantBufferView(3, 0, D3D12_ROOT_DESCRIPTOR_FLAG_NONE, D3D12_SHADER_VISIBILITY_DOMAIN);
		rootParameters[10].InitAsConstantBufferView(3, 0, D3D12_ROOT_DESCRIPTOR_FLAG_NONE, D3D12_SHADER_VISIBILITY_GEOMETRY);
		rootParameters[11].InitAsConstantBufferView(3, 0, D3D12_ROOT_DESCRIPTOR_FLAG_NONE, D3D12_SHADER_VISIBILITY_PIXEL);

		CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
		rootSignatureDesc.Init_1_1(_countof(rootParameters), rootParameters, _countof(samplerDesc), samplerDesc,
			D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
			D3D12_ROOT_SIGNATURE_FLAG_DENY_AMPLIFICATION_SHADER_ROOT_ACCESS | D3D12_ROOT_SIGNATURE_FLAG_DENY_MESH_SHADER_ROOT_ACCESS |
			D3D12_ROOT_SIGNATURE_FLAG_CBV_SRV_UAV_HEAP_DIRECTLY_INDEXED | D3D12_ROOT_SIGNATURE_FLAG_SAMPLER_HEAP_DIRECTLY_INDEXED);

		graphicsRootSignature = new RootSignature(rootSignatureDesc);
	}

	{
		//2+2+32+2 38 dwords
		CD3DX12_ROOT_PARAMETER1 rootParameters[4] = {};
		//reserved global constant buffer used by engine
		rootParameters[0].InitAsConstantBufferView(0, 0, D3D12_ROOT_DESCRIPTOR_FLAG_NONE, D3D12_SHADER_VISIBILITY_ALL);
		//user defined global per frame constant buffer
		rootParameters[1].InitAsConstantBufferView(1, 0, D3D12_ROOT_DESCRIPTOR_FLAG_NONE, D3D12_SHADER_VISIBILITY_ALL);
		//per dispatch call constants
		rootParameters[2].InitAsConstants(32, 2, 0, D3D12_SHADER_VISIBILITY_ALL);
		//per dispatch call constant buffers
		rootParameters[3].InitAsConstantBufferView(3, 0, D3D12_ROOT_DESCRIPTOR_FLAG_NONE, D3D12_SHADER_VISIBILITY_ALL);

		CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
		rootSignatureDesc.Init_1_1(_countof(rootParameters), rootParameters, _countof(samplerDesc), samplerDesc,
			D3D12_ROOT_SIGNATURE_FLAG_CBV_SRV_UAV_HEAP_DIRECTLY_INDEXED | D3D12_ROOT_SIGNATURE_FLAG_SAMPLER_HEAP_DIRECTLY_INDEXED);

		computeRootSignature = new RootSignature(rootSignatureDesc);
	}

	std::cout << "[class GlobalRootSignature] create global graphics and global compute root signatures succeeded\n";
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
