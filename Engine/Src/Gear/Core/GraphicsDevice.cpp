#include<Gear/Core/GraphicsDevice.h>

GraphicsDevice* GraphicsDevice::instance = nullptr;

ID3D12Device9* GraphicsDevice::get()
{
	return instance->device.Get();
}
