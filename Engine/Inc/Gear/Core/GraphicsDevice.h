#pragma once

#ifndef _GRAPHICSDEVICE_H_
#define _GRAPHICSDEVICE_H_

#include<Gear/Utils/Logger.h>

#include<D3D12Headers/d3dx12.h>

#include<dxgi1_6.h>

#include<wrl/client.h>

#include<comdef.h>

using Microsoft::WRL::ComPtr;

#define CHECKERROR(x) \
{\
HRESULT hr = x;\
if(FAILED(hr))\
{\
_com_error err(hr);\
LOGERROR("Failed with",std::hex,static_cast<uint32_t>(hr),std::dec,"Failed reason",err.ErrorMessage());\
}\
}\

//only works for power of 2
#define ROUND_UP(x,y) ((x + (y-1)) & ~(y-1))

class GraphicsDevice
{
public:

	GraphicsDevice(const GraphicsDevice&) = delete;

	void operator=(const GraphicsDevice&) = delete;

	static ID3D12Device9* get();

private:

	friend class RenderEngine;

	GraphicsDevice(IUnknown* const adapter);

	void checkFeatureSupport();

	static GraphicsDevice* instance;

	ComPtr<ID3D12Device9> device;

};

#endif // !_GRAPHICSDEVICE_H_