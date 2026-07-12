
//由 CommonShaderParse.exe 自动生成，请勿修改

#pragma once

#include<DirectXMath.h>

#include<cstdint>

namespace Gear
{
	namespace Core
	{
		namespace D3D12Core
		{
			namespace CommonShaderLayout
			{
				struct PerframeResource
				{
					float deltaTime;
					float timeElapsed;
					uint32_t uintSeed;
					float floatSeed;
					DirectX::XMMATRIX proj;
					DirectX::XMMATRIX view;
					DirectX::XMMATRIX prevViewProj;
					DirectX::XMMATRIX viewProj;
					DirectX::XMMATRIX normalMatrix;
					DirectX::XMFLOAT4 cameraPos;
					DirectX::XMFLOAT4 cameraRight;
					DirectX::XMFLOAT4 cameraUp;
					DirectX::XMFLOAT4 cameraForward;
					float fovAngleY;
					float zNear;
					float zFar;
					float aspectRatio;
					DirectX::XMFLOAT2 screenSize;
					DirectX::XMFLOAT2 screenTexelSize;
					DirectX::XMUINT4 padding1[5];
				};

				constexpr struct ShaderGlobalParameterIndices
				{
					uint32_t engineGlobalCBufferParameterIndex = 0;
					uint32_t userGlobalCBufferParameterIndex = 1;
				}globalParameterIndices;

				struct ShaderLocalParameterIndices
				{
					uint32_t perInvokeConstantsParameterIndex = 0;
					uint32_t perInvokeCBufferParameterIndex = 0;
				};

				constexpr uint32_t engineGlobalCBufferRegister = 0;
				constexpr uint32_t userGlobalCBufferRegister = 1;
				constexpr uint32_t perInvokeConstantsRegister = 2;
				constexpr uint32_t perInvokeCBufferRegister = 3;
			}
		}
	}
}