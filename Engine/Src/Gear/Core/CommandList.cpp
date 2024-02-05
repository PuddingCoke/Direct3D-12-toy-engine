#include<Gear/Core/CommandList.h>

CommandList::CommandList(const D3D12_COMMAND_LIST_TYPE type)
{
	for (UINT i = 0; i < Graphics::FrameBufferCount; i++)
	{
		allocators[i] = new CommandAllocator(type);
	}

	GraphicsDevice::get()->CreateCommandList(0, type, allocators[Graphics::getFrameIndex()]->get(), nullptr, IID_PPV_ARGS(&commandList));

	commandList->Close();
}

CommandList::~CommandList()
{
	for (UINT i = 0; i < Graphics::FrameBufferCount; i++)
	{
		if (allocators[i])
		{
			delete allocators[i];
		}
	}
}

void CommandList::reset()
{
	allocators[Graphics::getFrameIndex()]->get()->Reset();

	commandList->Reset(allocators[Graphics::getFrameIndex()]->get(), nullptr);
}

void CommandList::setDescriptorHeap(DescriptorHeap* const resourceHeap, DescriptorHeap* const samplerHeap)
{
	ID3D12DescriptorHeap* descriptorHeaps[2] = { resourceHeap->get(),samplerHeap->get() };

	commandList->SetDescriptorHeaps(2, descriptorHeaps);
}

void CommandList::setGraphicsRootSignature(RootSignature* const rootSignature)
{
	commandList->SetGraphicsRootSignature(rootSignature->get());
}

void CommandList::setComputeRootSignature(RootSignature* const rootSignature)
{
	commandList->SetComputeRootSignature(rootSignature->get());
}

ID3D12GraphicsCommandList6* CommandList::get()
{
	return commandList.Get();
}
