#include<Gear/Core/Shader.h>

#include<Gear/CompiledShaders/FullScreenVS.h>

#include<Gear/CompiledShaders/FullScreenPS.h>

#include<Gear/CompiledShaders/TextureCubeVS.h>

Shader* Shader::fullScreenVS = nullptr;

Shader* Shader::fullScreenPS = nullptr;

Shader* Shader::textureCubeVS = nullptr;

DXCCompiler* Shader::dxcCompiler = nullptr;

Shader::Shader(const uint8_t* const bytes, const size_t byteSize)
{
	shaderByteCode.pShaderBytecode = bytes;

	shaderByteCode.BytecodeLength = byteSize;
}

Shader::Shader(const std::wstring& filePath)
{
	if (Utils::File::getExtension(filePath) == L"cso")
	{
		bytes = Utils::File::readAllBinary(filePath);

		shaderByteCode.pShaderBytecode = bytes.data();

		shaderByteCode.BytecodeLength = bytes.size();

		LOGSUCCESS("read byte code at", Logger::brightBlue, filePath, Logger::resetColor(), "succeeded");
	}
	else
	{
		LOGERROR("input file's extension must be cso");
	}
}

Shader::Shader(const std::wstring& filePath, const ShaderProfile profile)
{
	if (Utils::File::getExtension(filePath) == L"hlsl")
	{
		shaderBlob = dxcCompiler->compile(filePath, profile);

		shaderByteCode.pShaderBytecode = shaderBlob->GetBufferPointer();

		shaderByteCode.BytecodeLength = shaderBlob->GetBufferSize();

		LOGSUCCESS("compile shader at", Logger::brightBlue, filePath, Logger::resetColor(), "succeeded");
	}
	else
	{
		LOGERROR("input file's extension must be hlsl");
	}
}

D3D12_SHADER_BYTECODE Shader::getByteCode() const
{
	return shaderByteCode;
}

void Shader::createGlobalShaders()
{
	dxcCompiler = new DXCCompiler();

	fullScreenVS = new Shader(g_FullScreenVSBytes, sizeof(g_FullScreenVSBytes));

	fullScreenPS = new Shader(g_FullScreenPSBytes, sizeof(g_FullScreenPSBytes));

	textureCubeVS = new Shader(g_TextureCubeVSBytes, sizeof(g_TextureCubeVSBytes));
}

void Shader::releaseGlobalShaders()
{
	if (dxcCompiler)
	{
		delete dxcCompiler;
	}

	if (fullScreenVS)
	{
		delete fullScreenVS;
	}

	if (fullScreenPS)
	{
		delete fullScreenPS;
	}

	if (textureCubeVS)
	{
		delete textureCubeVS;
	}
}

DXCCompiler::DXCCompiler()
{
	CHECKERROR(DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&dxcCompiler)));

	CHECKERROR(DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&dxcUtils)));

	CHECKERROR(dxcUtils->CreateDefaultIncludeHandler(&dxcIncludeHanlder));
}

DXCCompiler::~DXCCompiler()
{

}

IDxcBlob* DXCCompiler::compile(const std::wstring filePath, const ShaderProfile profile) const
{
	const std::vector<uint8_t> bytes = Utils::File::readAllBinary(filePath);
	
	ComPtr<IDxcBlobEncoding> textBlob;

	CHECKERROR(dxcUtils->CreateBlobFromPinned(bytes.data(), static_cast<uint32_t>(bytes.size()), codePage, &textBlob));

	DxcBuffer source = {};
	source.Ptr = textBlob->GetBufferPointer();
	source.Size = textBlob->GetBufferSize();
	source.Encoding = codePage;

	ComPtr<IDxcCompilerArgs> args;

	{
		switch (profile)
		{
		case VERTEX:
			dxcUtils->BuildArguments(filePath.c_str(), L"main", L"vs_6_6", nullptr, 0, nullptr, 0, &args);
			break;
		case HULL:
			dxcUtils->BuildArguments(filePath.c_str(), L"main", L"hs_6_6", nullptr, 0, nullptr, 0, &args);
			break;
		case DOMAIN:
			dxcUtils->BuildArguments(filePath.c_str(), L"main", L"ds_6_6", nullptr, 0, nullptr, 0, &args);
			break;
		case GEOMETRY:
			dxcUtils->BuildArguments(filePath.c_str(), L"main", L"gs_6_6", nullptr, 0, nullptr, 0, &args);
			break;
		case PIXEL:
			dxcUtils->BuildArguments(filePath.c_str(), L"main", L"ps_6_6", nullptr, 0, nullptr, 0, &args);
			break;
		case AMPLIFICATION:
			dxcUtils->BuildArguments(filePath.c_str(), L"main", L"as_6_6", nullptr, 0, nullptr, 0, &args);
			break;
		case MESH:
			dxcUtils->BuildArguments(filePath.c_str(), L"main", L"ms_6_6", nullptr, 0, nullptr, 0, &args);
			break;
		case COMPUTE:
			dxcUtils->BuildArguments(filePath.c_str(), L"main", L"cs_6_6", nullptr, 0, nullptr, 0, &args);
			break;
		case LIBRARY:
			dxcUtils->BuildArguments(filePath.c_str(), L"", L"lib_6_6", nullptr, 0, nullptr, 0, &args);
			break;
		default:
			LOGERROR("not supported shader profile");
			break;
		}
	}

	ComPtr<IDxcOperationResult> result;

	CHECKERROR(dxcCompiler->Compile(&source, args->GetArguments(), args->GetCount(), dxcIncludeHanlder.Get(), IID_PPV_ARGS(&result)));

	IDxcBlob* shaderBlob = nullptr;

	CHECKERROR(result->GetResult(&shaderBlob));

	return shaderBlob;
}
