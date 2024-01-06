#include<Gear/Core/CommandAllocator.h>

CommandAllocator::CommandAllocator(const D3D12_COMMAND_LIST_TYPE type)
{
	GraphicsDevice::get()->CreateCommandAllocator(type, IID_PPV_ARGS(&commandAllocator));
}

CommandAllocator::~CommandAllocator()
{
}

ID3D12CommandAllocator* CommandAllocator::get() const
{
	return commandAllocator.Get();
}
