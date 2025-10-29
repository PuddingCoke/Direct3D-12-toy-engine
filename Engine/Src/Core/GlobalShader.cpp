#include<Gear/Core/GlobalShader.h>

#include<Gear/Core/Internal/GlobalShaderInternal.h>

#include<Gear/CompiledShaders/FullScreenVS.h>

#include<Gear/CompiledShaders/FullScreenPS.h>

#include<Gear/CompiledShaders/TextureCubeVS.h>

namespace
{
	struct GlobalShaderPrivate
	{

		Gear::Core::D3D12Core::Shader* fullScreenVS = nullptr;

		Gear::Core::D3D12Core::Shader* fullScreenPS = nullptr;

		Gear::Core::D3D12Core::Shader* textureCubeVS = nullptr;

	}pvt;
}

Gear::Core::D3D12Core::Shader* Gear::Core::GlobalShader::getFullScreenVS()
{
	return pvt.fullScreenVS;
}

Gear::Core::D3D12Core::Shader* Gear::Core::GlobalShader::getFullScreenPS()
{
	return pvt.fullScreenPS;
}

Gear::Core::D3D12Core::Shader* Gear::Core::GlobalShader::getTextureCubeVS()
{
	return pvt.textureCubeVS;
}

void Gear::Core::GlobalShader::Internal::initialize()
{
	pvt.fullScreenVS = new D3D12Core::Shader(g_FullScreenVSBytes, sizeof(g_FullScreenVSBytes));

	pvt.fullScreenPS = new D3D12Core::Shader(g_FullScreenPSBytes, sizeof(g_FullScreenPSBytes));

	pvt.textureCubeVS = new D3D12Core::Shader(g_TextureCubeVSBytes, sizeof(g_TextureCubeVSBytes));
}

void Gear::Core::GlobalShader::Internal::release()
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