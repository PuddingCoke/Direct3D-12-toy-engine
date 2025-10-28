#pragma once

#ifndef _CORE_RESOURCE_SWAPTEXTURE_H_
#define _CORE_RESOURCE_SWAPTEXTURE_H_

#include"TextureRenderView.h"

#include<functional>

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

#endif // !_CORE_RESOURCE_SWAPTEXTURE_H_