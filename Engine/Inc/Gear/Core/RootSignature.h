#pragma once

#ifndef _ROOTSIGNATURE_H_
#define _ROOTSIGNATURE_H_

#include<Gear/Core/GraphicsDevice.h>

class RootSignature
{
public:

	RootSignature(const CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC& desc);

	RootSignature(const RootSignature&) = delete;

	void operator=(const RootSignature&) = delete;

	virtual ~RootSignature();

	ID3D12RootSignature* get() const;

private:

	ComPtr<ID3D12RootSignature> rootSignature;

};

#endif // !_ROOTSIGNATURE_H_