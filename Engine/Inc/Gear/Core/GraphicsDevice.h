#pragma once

#ifndef _GRAPHICSDEVICE_H_
#define _GRAPHICSDEVICE_H_

#include<D3D12Headers/d3dx12.h>
#include<wrl/client.h>
#include<iostream>
#include<comdef.h>

using Microsoft::WRL::ComPtr;

#define CHECKERROR(x) \
{\
HRESULT hr = x;\
if(FAILED(hr))\
{\
std::cout<<__FILE__<<" Line:"<<__LINE__<<"\n";\
std::cout<<"Function name "<<__FUNCTION__ <<"\n";\
std::cout<<"Failed with 0x"<<std::hex<<hr<<std::dec<<"\n";\
_com_error err(hr);\
std::wcout<<"Failed reason "<<err.ErrorMessage()<<"\n";\
__debugbreak();\
}\
}\

class GraphicsDevice
{
public:

	GraphicsDevice(const GraphicsDevice&) = delete;

	void operator=(const GraphicsDevice&) = delete;

	static ID3D12Device9* get();

private:

	friend class Gear;

	friend class RenderEngine;

	GraphicsDevice(IUnknown* const adapter);

	void checkFeatureSupport();

	static GraphicsDevice* instance;

	ComPtr<ID3D12Device9> device;

};

#endif // !_GRAPHICSDEVICE_H_
