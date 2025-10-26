#pragma once

#ifndef _CORE_GRAPHICSDEVICE_H_
#define _CORE_GRAPHICSDEVICE_H_

static_assert(sizeof(int) == 4, "size of int must be 4");

static_assert(sizeof(float) == 4, "size of float must be 4");

static_assert(sizeof(double) == 8, "size of double must be 8");

#include<Gear/Utils/Logger.h>

#include<D3D12Headers/d3dx12.h>

#include<dxgi1_6.h>

#include<wrl/client.h>

#include<comdef.h>

using Microsoft::WRL::ComPtr;

#define CHECKERROR(x) \
{\
const HRESULT hr = x;\
if(FAILED(hr))\
{\
_com_error err(hr);\
LOGERROR(L"Failed with",IntegerMode::HEX,static_cast<uint32_t>(hr),L"Failed reason",err.ErrorMessage());\
}\
}\

//only works for power of 2
#define ROUND_UP(x,y) ((x + (y-1)) & ~(y-1))

namespace Core
{
	namespace GraphicsDevice
	{
		ID3D12Device9* get();
	};
}

#endif // !_CORE_GRAPHICSDEVICE_H_