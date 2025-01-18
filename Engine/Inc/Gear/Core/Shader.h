#pragma once

#ifndef _SHADER_H_
#define _SHADER_H_

#include<Gear/Core/GraphicsDevice.h>

#include<Gear/Utils/Utils.h>

#include<Gear/CompiledShaders/FullScreenVS.h>
#include<Gear/CompiledShaders/FullScreenPS.h>
#include<Gear/CompiledShaders/TextureCubeVS.h>

#include<dxccompiler/dxcapi.h>

#undef DOMAIN

enum ShaderProfile
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

class DXCCompiler
{
public:

	DXCCompiler();

	~DXCCompiler();

	IDxcBlob* compile(const std::string filePath, const ShaderProfile profile) const;

private:

	static constexpr uint32_t codePage = CP_UTF8;

	ComPtr<IDxcCompiler3> dxcCompiler;

	ComPtr<IDxcUtils> dxcUtils;

	ComPtr<IDxcIncludeHandler> dxcIncludeHanlder;
	
};

class Shader
{
public:

	//byte code
	Shader(const BYTE* const bytes, const size_t byteSize);

	//cso
	Shader(const std::string& filePath);

	//hlsl
	//note:should copy Common.hlsl under Engine/Shaders to project directory if you want to use Common.hlsl
	Shader(const std::string& filePath, const ShaderProfile profile);

	D3D12_SHADER_BYTECODE getByteCode() const;

	static Shader* fullScreenVS;

	//context->setPSConstants({ resource }, 0);
	static Shader* fullScreenPS;

	static Shader* textureCubeVS;

private:

	friend class RenderEngine;

	D3D12_SHADER_BYTECODE shaderByteCode;

	//read byte code from file
	std::vector<BYTE> codes;

	//compiled code from file
	ComPtr<IDxcBlob> shaderBlob;

	static void createGlobalShaders();

	static void releaseGlobalShaders();

	static DXCCompiler* dxcCompiler;
	
};

#endif // !_SHADER_H_