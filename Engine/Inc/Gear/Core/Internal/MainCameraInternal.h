#pragma once

#ifndef _CORE_MAINCAMERA_INTERNAL_H_
#define _CORE_MAINCAMERA_INTERNAL_H_

#include<Gear/Utils/Math.h>

namespace Core
{
	namespace MainCamera
	{
		namespace Internal
		{
			struct Matrices
			{
				DirectX::XMMATRIX proj;
				DirectX::XMMATRIX view;
				DirectX::XMVECTOR eyePos;
				DirectX::XMMATRIX prevViewProj;
				DirectX::XMMATRIX viewProj;
				DirectX::XMMATRIX normalMatrix;
			} getMatrices();
		}
	}
}

#endif // !_CORE_MAINCAMERA_INTERNAL_H_
