#include<Gear/Core/Shader.h>

Shader* Shader::fullScreenVS = nullptr;

Shader* Shader::fullScreenPS = nullptr;

Shader* Shader::textureCubeVS = nullptr;

DXCCompiler* Shader::dxcCompiler = nullptr;

Shader::Shader(const BYTE* const bytes, const size_t byteSize)
{
	shaderByteCode.pShaderBytecode = bytes;

	shaderByteCode.BytecodeLength = byteSize;
}

Shader::Shader(const std::string& filePath)
{
	if (Utils::File::getExtension(filePath) == "cso")
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
	else
	{
		throw "not supported file format";
	}
}

Shader::Shader(const std::string& filePath, const ShaderProfile profile)
{
	if (Utils::File::getExtension(filePath) == "hlsl")
	{
		shaderBlob = dxcCompiler->compile(filePath, profile);

		shaderByteCode.pShaderBytecode = shaderBlob->GetBufferPointer();

		shaderByteCode.BytecodeLength = shaderBlob->GetBufferSize();

		std::cout << "[class Shader] compile shader at " << filePath << " succeeded\n";
	}
	else
	{
		throw "not supported file format";
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

IDxcBlob* DXCCompiler::compile(const std::string filePath,const ShaderProfile profile) const
{
	const std::string shaderString = Utils::File::readAllText(filePath);

	ComPtr<IDxcBlobEncoding> textBlob;

	CHECKERROR(dxcUtils->CreateBlobFromPinned(shaderString.c_str(), shaderString.size(), codePage, &textBlob));

	DxcBuffer source = {};
	source.Ptr = textBlob->GetBufferPointer();
	source.Size = textBlob->GetBufferSize();
	source.Encoding = codePage;

	ComPtr<IDxcCompilerArgs> args;

	{
		const std::wstring wFilePath = std::wstring(filePath.begin(), filePath.end());

		switch (profile)
		{
		case VERTEX:
			dxcUtils->BuildArguments(wFilePath.c_str(), L"main", L"vs_6_6", nullptr, 0, nullptr, 0, &args);
			break;
		case HULL:
			dxcUtils->BuildArguments(wFilePath.c_str(), L"main", L"hs_6_6", nullptr, 0, nullptr, 0, &args);
			break;
		case DOMAIN:
			dxcUtils->BuildArguments(wFilePath.c_str(), L"main", L"ds_6_6", nullptr, 0, nullptr, 0, &args);
			break;
		case GEOMETRY:
			dxcUtils->BuildArguments(wFilePath.c_str(), L"main", L"gs_6_6", nullptr, 0, nullptr, 0, &args);
			break;
		case PIXEL:
			dxcUtils->BuildArguments(wFilePath.c_str(), L"main", L"ps_6_6", nullptr, 0, nullptr, 0, &args);
			break;
		case AMPLIFICATION:
			dxcUtils->BuildArguments(wFilePath.c_str(), L"main", L"as_6_6", nullptr, 0, nullptr, 0, &args);
			break;
		case MESH:
			dxcUtils->BuildArguments(wFilePath.c_str(), L"main", L"ms_6_6", nullptr, 0, nullptr, 0, &args);
			break;
		case COMPUTE:
			dxcUtils->BuildArguments(wFilePath.c_str(), L"main", L"cs_6_6", nullptr, 0, nullptr, 0, &args);
			break;
		case LIBRARY:
			dxcUtils->BuildArguments(wFilePath.c_str(), L"", L"lib_6_6", nullptr, 0, nullptr, 0, &args);
			break;
		default:
			throw "not supported profile";
			break;
		}
	}

	ComPtr<IDxcOperationResult> result;

	CHECKERROR(dxcCompiler->Compile(&source, args->GetArguments(), args->GetCount(), dxcIncludeHanlder.Get(), IID_PPV_ARGS(&result)));

	IDxcBlob* shaderBlob = nullptr;

	CHECKERROR(result->GetResult(&shaderBlob));

	return shaderBlob;
}
