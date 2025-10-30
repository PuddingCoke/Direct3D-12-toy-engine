﻿#pragma once

#ifndef _GEAR_CORE_RENDERENGINE_H_
#define _GEAR_CORE_RENDERENGINE_H_

#include<Gear/Core/D3D12Core/CommandList.h>

namespace Gear
{
	namespace Core
	{
		enum class GPUVendor
		{
			NVIDIA,
			AMD,
			INTEL,
			UNKNOWN
		};

		namespace RenderEngine
		{

			void submitCommandList(D3D12Core::CommandList* const commandList);

			GPUVendor getVendor();

			Resource::D3D12Resource::Texture* getRenderTexture();

			bool getDisplayImGuiSurface();

		}
	}
}

#endif // !_GEAR_CORE_RENDERENGINE_H_