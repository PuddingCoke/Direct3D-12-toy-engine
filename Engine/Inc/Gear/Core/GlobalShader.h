#pragma once

#ifndef _CORE_GLOBALSHADER_H_
#define _CORE_GLOBALSHADER_H_

#include"D3D12Core/Shader.h"

namespace Core
{
	namespace GlobalShader
	{
		D3D12Core::Shader* getFullScreenVS();

		D3D12Core::Shader* getFullScreenPS();

		D3D12Core::Shader* getTextureCubeVS();
	}
}

#endif // !_CORE_GLOBALSHADER_H_
