#pragma once

#ifndef _SHADER_H_
#define _SHADER_H_

#include<Gear/Core/GraphicsDevice.h>

#include<Gear/Utils/Utils.h>

class Shader
{
public:

	Shader(const BYTE* const bytes, const size_t byteSize);

	Shader(const std::string filePath);

	D3D12_SHADER_BYTECODE getByteCode() const;

private:

	D3D12_SHADER_BYTECODE shaderByteCode;

	std::vector<char> codes;

};



#endif // !_SHADER_H_
