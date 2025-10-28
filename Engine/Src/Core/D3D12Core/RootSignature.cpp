#include<Gear/Core/D3D12Core/RootSignature.h>

Core::D3D12Core::RootSignature::RootSignature(const CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC& desc)
{
	ComPtr<ID3DBlob> signature;

	ComPtr<ID3DBlob> error;

	D3DX12SerializeVersionedRootSignature(&desc, D3D_ROOT_SIGNATURE_VERSION_1_1, &signature, &error);

	CHECKERROR(Core::GraphicsDevice::get()->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&rootSignature)));
}

Core::D3D12Core::RootSignature::~RootSignature()
{
}

ID3D12RootSignature* Core::D3D12Core::RootSignature::get() const
{
	return rootSignature.Get();
}
