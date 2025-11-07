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

				RootSignature(
					const uint32_t numVSConstants,
					const uint32_t numHSConstants,
					const uint32_t numDSConstants,
					const uint32_t numGSConstants,
					const uint32_t numPSConstants,
					const uint32_t numCSConstants,
					CD3DX12_STATIC_SAMPLER_DESC* const samplerDescs, const uint32_t samplerCount,
					const D3D12_ROOT_SIGNATURE_FLAGS signatureFlags);

				virtual ~RootSignature();

				ID3D12RootSignature* get() const;

				static uint32_t getReservedGlobalConstantBufferParameterIndex();

				static uint32_t getUserDefinedGlobalConstantBufferParameterIndex();

				uint32_t getVSConstantsParameterIndex() const;

				uint32_t getVSConstantBufferParameterIndex() const;

				uint32_t getHSConstantsParameterIndex() const;

				uint32_t getHSConstantBufferParameterIndex() const;

				uint32_t getDSConstantsParameterIndex() const;

				uint32_t getDSConstantBufferParameterIndex() const;

				uint32_t getGSConstantsParameterIndex() const;

				uint32_t getGSConstantBufferParameterIndex() const;

				uint32_t getPSConstantsParameterIndex() const;

				uint32_t getPSConstantBufferParameterIndex() const;

				uint32_t getCSConstantsParameterIndex() const;

				uint32_t getCSConstantBufferParameterIndex() const;

			private:

				enum class ShaderType
				{
					VERTEX,
					HULL,
					DOMAIN,
					GEOMETRY,
					PIXEL,
					COMPUTE,
					TYPECOUNT
				};

				uint32_t getShaderConstantsParameterIndex(const ShaderType shaderType) const;

				ComPtr<ID3D12RootSignature> rootSignature;

				//存储每个着色器的32位常量数量，顺序为V H D G P C，它会被用来检测错误
				const uint32_t numConstants[static_cast<uint32_t>(ShaderType::TYPECOUNT)];

				//存储每个着色器的根参数的开始位置，顺序同上
				uint32_t rootParameterStartIndices[static_cast<uint32_t>(ShaderType::TYPECOUNT)];

			};
		}
	}
}

#endif // !_GEAR_CORE_D3D12CORE_ROOTSIGNATURE_H_