#pragma once

#ifndef _CORE_GRAPHICSDEVICE_INTERNAL_H_
#define _CORE_GRAPHICSDEVICE_INTERNAL_H_

#include<D3D12Headers/d3dx12.h>

namespace Core
{
	namespace GraphicsDevice
	{
		namespace Internal
		{
			void initialize(IUnknown* const adapter);

			void release();

			void checkFeatureSupport();
		}
	}
}

#endif // !_CORE_GRAPHICSDEVICE_INTERNAL_H_
