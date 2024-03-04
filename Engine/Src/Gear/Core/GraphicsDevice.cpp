#include<Gear/Core/GraphicsDevice.h>

GraphicsDevice* GraphicsDevice::instance = nullptr;

ID3D12Device9* GraphicsDevice::get()
{
	return instance->device.Get();
}

GraphicsDevice::GraphicsDevice(IUnknown* const adapter)
{
	if (SUCCEEDED(D3D12CreateDevice(adapter, D3D_FEATURE_LEVEL_12_2, IID_PPV_ARGS(&device))))
	{
		std::cout<<"[class GraphicsDevice] create d3d12 device at feature level 12_2 succeeded\n";
	}
}
