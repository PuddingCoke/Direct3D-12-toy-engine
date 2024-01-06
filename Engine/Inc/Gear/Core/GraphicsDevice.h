#pragma once

#ifndef _GRAPHICSDEVICE_H_
#define _GRAPHICSDEVICE_H_

#include<D3D12Headers/d3dx12.h>
#include<wrl/client.h>

using Microsoft::WRL::ComPtr;

class GraphicsDevice
{
public:

	GraphicsDevice(const GraphicsDevice&) = delete;

	void operator=(const GraphicsDevice&) = delete;

	static ID3D12Device9* get();

private:

	friend class Gear;

	GraphicsDevice();

	static GraphicsDevice* instance;

	ComPtr<ID3D12Device9> device;

};

#endif // !_GRAPHICSDEVICE_H_
