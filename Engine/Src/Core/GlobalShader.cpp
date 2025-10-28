#include<Gear/Core/GlobalShader.h>

#include<Gear/Core/Internal/GlobalShaderInternal.h>

#include<Gear/CompiledShaders/FullScreenVS.h>

#include<Gear/CompiledShaders/FullScreenPS.h>

#include<Gear/CompiledShaders/TextureCubeVS.h>

namespace
{
	struct GlobalShaderPrivate
	{

		Core::D3D12Core::Shader* fullScreenVS = nullptr;

		Core::D3D12Core::Shader* fullScreenPS = nullptr;

		Core::D3D12Core::Shader* textureCubeVS = nullptr;

	}pvt;
}

Core::D3D12Core::Shader* Core::GlobalShader::getFullScreenVS()
{
	return pvt.fullScreenVS;
}

Core::D3D12Core::Shader* Core::GlobalShader::getFullScreenPS()
{
	return pvt.fullScreenPS;
}

Core::D3D12Core::Shader* Core::GlobalShader::getTextureCubeVS()
{
	return pvt.textureCubeVS;
}

void Core::GlobalShader::Internal::initialize()
{
	pvt.fullScreenVS = new D3D12Core::Shader(g_FullScreenVSBytes, sizeof(g_FullScreenVSBytes));

	pvt.fullScreenPS = new D3D12Core::Shader(g_FullScreenPSBytes, sizeof(g_FullScreenPSBytes));

	pvt.textureCubeVS = new D3D12Core::Shader(g_TextureCubeVSBytes, sizeof(g_TextureCubeVSBytes));
}

void Core::GlobalShader::Internal::release()
{
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