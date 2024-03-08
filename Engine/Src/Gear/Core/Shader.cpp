#include<Gear/Core/Shader.h>

Shader* Shader::fullScreenVS = nullptr;

Shader::Shader(const BYTE* const bytes, const size_t byteSize)
{
	shaderByteCode.pShaderBytecode = bytes;
	shaderByteCode.BytecodeLength = byteSize;

	std::cout << "[class Shader] read byte code succeeded\n";
}

Shader::Shader(const std::string filePath)
{
	std::ifstream file(filePath, std::ios::ate | std::ios::binary);

	if (!file.is_open())
	{
		throw "cannot open file";
	}

	size_t fileSize = static_cast<size_t>(file.tellg());

	codes = std::vector<BYTE>(fileSize);

	file.seekg(0);

	file.read(reinterpret_cast<char*>(codes.data()), fileSize);

	file.close();

	shaderByteCode.pShaderBytecode = codes.data();
	shaderByteCode.BytecodeLength = codes.size();

	std::cout << "[class Shader] read byte code at " << filePath << " succeeded\n";
}

D3D12_SHADER_BYTECODE Shader::getByteCode() const
{
	return shaderByteCode;
}

void Shader::createGlobalShaders()
{
	fullScreenVS = new Shader(g_FullScreenVSBytes, sizeof(g_FullScreenVSBytes));
}

void Shader::releaseGlobalShaders()
{
	if (fullScreenVS)
	{
		delete fullScreenVS;
	}
}
