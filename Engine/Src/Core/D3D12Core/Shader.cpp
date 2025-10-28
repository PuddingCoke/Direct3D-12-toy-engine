#include<Gear/Core/D3D12Core/Shader.h>

#include<Gear/Utils/File.h>

Core::D3D12Core::Shader::Shader(const uint8_t* const bytes, const size_t byteSize)
{
	shaderByteCode.pShaderBytecode = bytes;

	shaderByteCode.BytecodeLength = byteSize;
}

Core::D3D12Core::Shader::Shader(const std::wstring& filePath)
{
	if (Utils::File::getExtension(filePath) == L"cso")
	{
		shaderBlob = DXCCompiler::read(filePath);

		shaderByteCode.pShaderBytecode = shaderBlob->GetBufferPointer();

		shaderByteCode.BytecodeLength = shaderBlob->GetBufferSize();

		LOGSUCCESS(L"read byte code at", LogColor::brightBlue, filePath, LogColor::defaultColor, L"succeeded");
	}
	else
	{
		LOGERROR(L"input file's extension must be cso");
	}
}

Core::D3D12Core::Shader::Shader(const std::wstring& filePath, const DXCCompiler::ShaderProfile profile)
{
	if (Utils::File::getExtension(filePath) == L"hlsl")
	{
		shaderBlob = DXCCompiler::compile(filePath, profile);

		shaderByteCode.pShaderBytecode = shaderBlob->GetBufferPointer();

		shaderByteCode.BytecodeLength = shaderBlob->GetBufferSize();

		LOGSUCCESS(L"compile shader at", LogColor::brightBlue, filePath, LogColor::defaultColor, L"succeeded");
	}
	else
	{
		LOGERROR(L"input file's extension must be hlsl");
	}
}

D3D12_SHADER_BYTECODE Core::D3D12Core::Shader::getByteCode() const
{
	return shaderByteCode;
}
