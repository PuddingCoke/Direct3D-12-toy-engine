#include<Gear/Core/GlobalRootSignature.h>

GlobalRootSignature::GlobalRootSignature()
{
	CD3DX12_DESCRIPTOR_RANGE1 ranges[3] = {};

	ranges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, UINT_MAX, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_NONE, 0);
	ranges[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, UINT_MAX, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_NONE, 0);
	ranges[2].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, UINT_MAX, 3, 0, D3D12_DESCRIPTOR_RANGE_FLAG_NONE, 0);

	CD3DX12_ROOT_PARAMETER1 rootParameters[11];
	rootParameters[0].InitAsDescriptorTable(3, &ranges[0], D3D12_SHADER_VISIBILITY_ALL);

	//reserved descriptor for global constant buffer that can be accessed by any shader stage at any time
	rootParameters[1].InitAsConstantBufferView(0, 0, D3D12_ROOT_DESCRIPTOR_FLAG_NONE, D3D12_SHADER_VISIBILITY_ALL);

	//user defined global constant buffer that can be accessed by any shader stage within one frame
	rootParameters[2].InitAsConstantBufferView(1, 0, D3D12_ROOT_DESCRIPTOR_FLAG_NONE, D3D12_SHADER_VISIBILITY_ALL);

	rootParameters[3].InitAsConstants(16, 2, 0, D3D12_SHADER_VISIBILITY_ALL);

	//user defined per stage per draw call constant buffer
	rootParameters[4].InitAsConstantBufferView(3, 0, D3D12_ROOT_DESCRIPTOR_FLAG_NONE, D3D12_SHADER_VISIBILITY_AMPLIFICATION);
	rootParameters[5].InitAsConstantBufferView(3, 0, D3D12_ROOT_DESCRIPTOR_FLAG_NONE, D3D12_SHADER_VISIBILITY_DOMAIN);
	rootParameters[6].InitAsConstantBufferView(3, 0, D3D12_ROOT_DESCRIPTOR_FLAG_NONE, D3D12_SHADER_VISIBILITY_GEOMETRY);
	rootParameters[7].InitAsConstantBufferView(3, 0, D3D12_ROOT_DESCRIPTOR_FLAG_NONE, D3D12_SHADER_VISIBILITY_HULL);
	rootParameters[8].InitAsConstantBufferView(3, 0, D3D12_ROOT_DESCRIPTOR_FLAG_NONE, D3D12_SHADER_VISIBILITY_MESH);
	rootParameters[9].InitAsConstantBufferView(3, 0, D3D12_ROOT_DESCRIPTOR_FLAG_NONE, D3D12_SHADER_VISIBILITY_PIXEL);
	rootParameters[10].InitAsConstantBufferView(3, 0, D3D12_ROOT_DESCRIPTOR_FLAG_NONE, D3D12_SHADER_VISIBILITY_VERTEX);
}
