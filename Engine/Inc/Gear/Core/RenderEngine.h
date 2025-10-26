#pragma once

#ifndef _CORE_RENDERENGINE_H_
#define _CORE_RENDERENGINE_H_

#include<Gear/Core/CommandList.h>

#include<Gear/Core/Resource/D3D12Resource/ReadbackHeap.h>

#include<Gear/Core/ResourceManager.h>

#include<ImGUI/imgui.h>

#include<ImGUI/imgui_impl_win32.h>

#include<ImGUI/imgui_impl_dx12.h>

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

		void submitCommandList(CommandList* const commandList);

		GPUVendor getVendor();

		Texture* getRenderTexture();

		bool getDisplayImGuiSurface();

	}
}

#endif // !_RENDERENGINE_H_