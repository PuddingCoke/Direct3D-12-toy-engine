#include<Gear/Core/Shader.h>

Shader::Shader(const BYTE* const bytes, const size_t byteSize)
{
	shaderByteCode.pShaderBytecode = bytes;
	shaderByteCode.BytecodeLength = byteSize;
}

Shader::Shader(const std::string filePath)
{
	std::ifstream file(filePath, std::ios::ate | std::ios::binary);

	if (!file.is_open())
	{
		throw "cannot open file";
	}

	size_t fileSize = (size_t)file.tellg();

	codes = std::vector<char>(fileSize);

	file.seekg(0);

	file.read(codes.data(), fileSize);

	file.close();

	shaderByteCode.pShaderBytecode = codes.data();
	shaderByteCode.BytecodeLength = codes.size();

	std::cout << "[class Shader] read byte code at " << filePath << " succeeded\n";
}

D3D12_SHADER_BYTECODE Shader::getByteCode() const
{
	return shaderByteCode;
}
