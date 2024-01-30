#include<Gear/Core/CommandList.h>

CommandList::CommandList(const D3D12_COMMAND_LIST_TYPE type) :
	allocators(new CommandAllocator[GraphicsSettings::FrameBufferCount]{ CommandAllocator(type),CommandAllocator(type),CommandAllocator(type)})
{
	GraphicsDevice::get()->CreateCommandList(0, type, allocators[GraphicsSettings::getFrameIndex()].get(), nullptr, IID_PPV_ARGS(&commandList));

	commandList->Close();
}

CommandList::~CommandList()
{
	delete[] allocators;
}

void CommandList::Reset()
{
	allocators[GraphicsSettings::getFrameIndex()].get()->Reset();

	commandList->Reset(allocators[GraphicsSettings::getFrameIndex()].get(), nullptr);
}

ID3D12GraphicsCommandList7* CommandList::get()
{
	return commandList.Get();
}
