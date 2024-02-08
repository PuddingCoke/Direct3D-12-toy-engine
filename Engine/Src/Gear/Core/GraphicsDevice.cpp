#include<Gear/Core/GraphicsDevice.h>

GraphicsDevice* GraphicsDevice::instance = nullptr;

ID3D12Device9* GraphicsDevice::get()
{
	return instance->device.Get();
}

GraphicsDevice::GraphicsDevice(IUnknown* const adapter)
{
	D3D12CreateDevice(adapter, D3D_FEATURE_LEVEL_12_2, IID_PPV_ARGS(&device));
}
