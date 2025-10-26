#pragma once

#ifndef _CORE_SHADER_H_
#define _CORE_SHADER_H_

#include<Gear/Core/GraphicsDevice.h>

#include<dxccompiler/dxcapi.h>

#undef DOMAIN

namespace Core
{
	enum class ShaderProfile
	{
		VERTEX,
		HULL,
		DOMAIN,
		GEOMETRY,
		PIXEL,
		AMPLIFICATION,
		MESH,
		COMPUTE,
		LIBRARY
	};

	class Shader
	{
	public:

		Shader() = delete;

		Shader(const Shader&) = delete;

		void operator=(const Shader&) = delete;

		//byte code
		Shader(const uint8_t* const bytes, const size_t byteSize);

		//cso
		Shader(const std::wstring& filePath);

		//hlsl
		//note:should copy Common.hlsl under Engine/Shaders to project directory if you want to use Common.hlsl
		Shader(const std::wstring& filePath, const ShaderProfile profile);

		D3D12_SHADER_BYTECODE getByteCode() const;

	private:

		D3D12_SHADER_BYTECODE shaderByteCode;

		ComPtr<IDxcBlob> shaderBlob;

	};

	namespace GlobalShader
	{
		Shader* getFullScreenVS();

		Shader* getFullScreenPS();

		Shader* getTextureCubeVS();
	}
}

#endif // !_CORE_SHADER_H_