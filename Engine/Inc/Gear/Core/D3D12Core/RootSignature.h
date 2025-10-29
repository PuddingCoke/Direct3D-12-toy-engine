#pragma once

#ifndef _GEAR_CORE_D3D12CORE_ROOTSIGNATURE_H_
#define _GEAR_CORE_D3D12CORE_ROOTSIGNATURE_H_

#include<Gear/Core/GraphicsDevice.h>

namespace Gear
{
	namespace Core
	{
		namespace D3D12Core
		{
			class RootSignature
			{
			public:

				RootSignature() = delete;

				RootSignature(const RootSignature&) = delete;

				void operator=(const RootSignature&) = delete;

				RootSignature(const CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC& desc);

				virtual ~RootSignature();

				ID3D12RootSignature* get() const;

			private:

				ComPtr<ID3D12RootSignature> rootSignature;

			};
		}
	}
}

#endif // !_GEAR_CORE_D3D12CORE_ROOTSIGNATURE_H_