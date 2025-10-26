#include<Gear/Core/Shader.h>

#include<Gear/Core/Internal/GlobalShaderInternal.h>

#include<Gear/Utils/File.h>

#include<Gear/CompiledShaders/FullScreenVS.h>

#include<Gear/CompiledShaders/FullScreenPS.h>

#include<Gear/CompiledShaders/TextureCubeVS.h>

namespace
{
	class DXCCompiler
	{
	public:

		DXCCompiler(const DXCCompiler&) = delete;

		void operator=(const DXCCompiler&) = delete;

		DXCCompiler();

		~DXCCompiler();

		//hlsl
		ComPtr<IDxcBlob> compile(const std::wstring& filePath, const Core::ShaderProfile profile) const;

		//cso
		ComPtr<IDxcBlob> read(const std::wstring& filePath) const;

	private:

		static constexpr uint32_t codePage = CP_UTF8;

		ComPtr<IDxcCompiler3> dxcCompiler;

		ComPtr<IDxcUtils> dxcUtils;

		ComPtr<IDxcIncludeHandler> dxcIncludeHanlder;

	}*dxcCompiler = nullptr;

	struct GlobalShaderPrivate
	{

		Core::Shader* fullScreenVS = nullptr;

		Core::Shader* fullScreenPS = nullptr;

		Core::Shader* textureCubeVS = nullptr;

	}pvt;

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

ComPtr<IDxcBlob> DXCCompiler::compile(const std::wstring& filePath, const Core::ShaderProfile profile) const
{
	const std::vector<uint8_t> bytes = Utils::File::readAllBinary(filePath);

	ComPtr<IDxcBlobEncoding> textBlob;

	CHECKERROR(dxcUtils->CreateBlobFromPinned(bytes.data(), static_cast<uint32_t>(bytes.size()), codePage, &textBlob));

	DxcBuffer source = {};
	source.Ptr = textBlob->GetBufferPointer();
	source.Size = textBlob->GetBufferSize();
	source.Encoding = codePage;

	ComPtr<IDxcCompilerArgs> args;

	switch (profile)
	{
	case Core::ShaderProfile::VERTEX:
		dxcUtils->BuildArguments(filePath.c_str(), L"main", L"vs_6_6", nullptr, 0, nullptr, 0, &args);
		break;
	case Core::ShaderProfile::HULL:
		dxcUtils->BuildArguments(filePath.c_str(), L"main", L"hs_6_6", nullptr, 0, nullptr, 0, &args);
		break;
	case Core::ShaderProfile::DOMAIN:
		dxcUtils->BuildArguments(filePath.c_str(), L"main", L"ds_6_6", nullptr, 0, nullptr, 0, &args);
		break;
	case Core::ShaderProfile::GEOMETRY:
		dxcUtils->BuildArguments(filePath.c_str(), L"main", L"gs_6_6", nullptr, 0, nullptr, 0, &args);
		break;
	case Core::ShaderProfile::PIXEL:
		dxcUtils->BuildArguments(filePath.c_str(), L"main", L"ps_6_6", nullptr, 0, nullptr, 0, &args);
		break;
	case Core::ShaderProfile::AMPLIFICATION:
		dxcUtils->BuildArguments(filePath.c_str(), L"main", L"as_6_6", nullptr, 0, nullptr, 0, &args);
		break;
	case Core::ShaderProfile::MESH:
		dxcUtils->BuildArguments(filePath.c_str(), L"main", L"ms_6_6", nullptr, 0, nullptr, 0, &args);
		break;
	case Core::ShaderProfile::COMPUTE:
		dxcUtils->BuildArguments(filePath.c_str(), L"main", L"cs_6_6", nullptr, 0, nullptr, 0, &args);
		break;
	case Core::ShaderProfile::LIBRARY:
		dxcUtils->BuildArguments(filePath.c_str(), L"", L"lib_6_6", nullptr, 0, nullptr, 0, &args);
		break;
	default:
		LOGERROR(L"not supported shader profile");
		break;
	}

	ComPtr<IDxcOperationResult> result;

	CHECKERROR(dxcCompiler->Compile(&source, args->GetArguments(), args->GetCount(), dxcIncludeHanlder.Get(), IID_PPV_ARGS(&result)));

	ComPtr<IDxcBlob> shaderBlob;

	CHECKERROR(result->GetResult(&shaderBlob));

	return shaderBlob;
}

ComPtr<IDxcBlob> DXCCompiler::read(const std::wstring& filePath) const
{
	const std::vector<uint8_t> bytes = Utils::File::readAllBinary(filePath);

	ComPtr<IDxcBlobEncoding> textBlob;

	CHECKERROR(dxcUtils->CreateBlob(bytes.data(), static_cast<uint32_t>(bytes.size()), CP_NONE, &textBlob));

	ComPtr<IDxcBlob> shaderBlob = textBlob;

	return shaderBlob;
}

Core::Shader::Shader(const uint8_t* const bytes, const size_t byteSize)
{
	shaderByteCode.pShaderBytecode = bytes;

	shaderByteCode.BytecodeLength = byteSize;
}

Core::Shader::Shader(const std::wstring& filePath)
{
	if (Utils::File::getExtension(filePath) == L"cso")
	{
		shaderBlob = dxcCompiler->read(filePath);

		shaderByteCode.pShaderBytecode = shaderBlob->GetBufferPointer();

		shaderByteCode.BytecodeLength = shaderBlob->GetBufferSize();

		LOGSUCCESS(L"read byte code at", LogColor::brightBlue, filePath, LogColor::defaultColor, L"succeeded");
	}
	else
	{
		LOGERROR(L"input file's extension must be cso");
	}
}

Core::Shader::Shader(const std::wstring& filePath, const ShaderProfile profile)
{
	if (Utils::File::getExtension(filePath) == L"hlsl")
	{
		shaderBlob = dxcCompiler->compile(filePath, profile);

		shaderByteCode.pShaderBytecode = shaderBlob->GetBufferPointer();

		shaderByteCode.BytecodeLength = shaderBlob->GetBufferSize();

		LOGSUCCESS(L"compile shader at", LogColor::brightBlue, filePath, LogColor::defaultColor, L"succeeded");
	}
	else
	{
		LOGERROR(L"input file's extension must be hlsl");
	}
}

D3D12_SHADER_BYTECODE Core::Shader::getByteCode() const
{
	return shaderByteCode;
}

Core::Shader* Core::GlobalShader::getFullScreenVS()
{
	return pvt.fullScreenVS;
}

Core::Shader* Core::GlobalShader::getFullScreenPS()
{
	return pvt.fullScreenPS;
}

Core::Shader* Core::GlobalShader::getTextureCubeVS()
{
	return pvt.textureCubeVS;
}

void Core::GlobalShader::Internal::initialize()
{
	dxcCompiler = new DXCCompiler();

	pvt.fullScreenVS = new Shader(g_FullScreenVSBytes, sizeof(g_FullScreenVSBytes));

	pvt.fullScreenPS = new Shader(g_FullScreenPSBytes, sizeof(g_FullScreenPSBytes));

	pvt.textureCubeVS = new Shader(g_TextureCubeVSBytes, sizeof(g_TextureCubeVSBytes));
}

void Core::GlobalShader::Internal::release()
{
	if (dxcCompiler)
	{
		delete dxcCompiler;
	}

	if (pvt.fullScreenVS)
	{
		delete pvt.fullScreenVS;
	}

	if (pvt.fullScreenPS)
	{
		delete pvt.fullScreenPS;
	}

	if (pvt.textureCubeVS)
	{
		delete pvt.textureCubeVS;
	}
}
