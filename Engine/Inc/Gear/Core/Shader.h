﻿#pragma once

#ifndef _SHADER_H_
#define _SHADER_H_

#include<Gear/Core/GraphicsDevice.h>

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

	DXCCompiler(const DXCCompiler&) = delete;

	void operator=(const DXCCompiler&) = delete;

	DXCCompiler();

	~DXCCompiler();

	//hlsl
	ComPtr<IDxcBlob> compile(const std::wstring& filePath, const ShaderProfile profile) const;

	//cso
	ComPtr<IDxcBlob> read(const std::wstring& filePath) const;

private:

	static constexpr uint32_t codePage = CP_UTF8;

	ComPtr<IDxcCompiler3> dxcCompiler;

	ComPtr<IDxcUtils> dxcUtils;

	ComPtr<IDxcIncludeHandler> dxcIncludeHanlder;
	
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

	static Shader* fullScreenVS;

	//context->setPSConstants({ resource }, 0);
	static Shader* fullScreenPS;

	static Shader* textureCubeVS;

private:

	friend class RenderEngine;

	D3D12_SHADER_BYTECODE shaderByteCode;

	ComPtr<IDxcBlob> shaderBlob;

	static void createGlobalShaders();

	static void releaseGlobalShaders();

	static DXCCompiler* dxcCompiler;
	
};

#endif // !_SHADER_H_