#pragma once

#ifndef _SHADER_H_
#define _SHADER_H_

#include<Gear/Core/GraphicsDevice.h>

#include<Gear/Utils/Utils.h>

#include<Gear/CompiledShaders/FullScreenVS.h>
#include<Gear/CompiledShaders/FullScreenPS.h>
#include<Gear/CompiledShaders/TextureCubeVS.h>

class Shader
{
public:

	Shader(const BYTE* const bytes, const size_t byteSize);

	Shader(const std::string filePath);

	D3D12_SHADER_BYTECODE getByteCode() const;

	static Shader* fullScreenVS;

	static Shader* fullScreenPS;

	static Shader* textureCubeVS;

private:

	friend class RenderEngine;

	D3D12_SHADER_BYTECODE shaderByteCode;

	std::vector<BYTE> codes;

	static void createGlobalShaders();

	static void releaseGlobalShaders();

};



#endif // !_SHADER_H_
