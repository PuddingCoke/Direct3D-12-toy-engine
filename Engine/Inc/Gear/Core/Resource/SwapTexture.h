#pragma once

#ifndef _GEAR_CORE_RESOURCE_SWAPTEXTURE_H_
#define _GEAR_CORE_RESOURCE_SWAPTEXTURE_H_

#include"TextureRenderView.h"

#include<functional>

namespace Gear
{
	namespace Core
	{
		namespace Resource
		{
			class SwapTexture
			{
			private:

				TextureRenderView* texture0;

				TextureRenderView* texture1;

			public:

				SwapTexture() = delete;

				SwapTexture(const std::function<TextureRenderView* (void)>& factoryFunc);

				~SwapTexture();

				TextureRenderView* read() const;

				TextureRenderView* write() const;

				void swap();

				const uint32_t width;

				const uint32_t height;

			};
		}
	}
}

#endif // !_GEAR_CORE_RESOURCE_SWAPTEXTURE_H_