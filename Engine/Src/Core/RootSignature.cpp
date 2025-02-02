﻿#include<Gear/Core/RootSignature.h>

RootSignature::RootSignature(const CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC& desc)
{
	ComPtr<ID3DBlob> signature;

	ComPtr<ID3DBlob> error;

	D3DX12SerializeVersionedRootSignature(&desc, D3D_ROOT_SIGNATURE_VERSION_1_1, &signature, &error);

	CHECKERROR(GraphicsDevice::get()->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&rootSignature)));
}

RootSignature::~RootSignature()
{
}

ID3D12RootSignature* RootSignature::get() const
{
	return rootSignature.Get();
}
