#pragma once

#ifndef _COMMANDLIST_H_
#define _COMMANDLIST_H_

#include<Gear/Core/CommandAllocator.h>
#include<Gear/Core/Graphics.h>
#include<Gear/Core/GlobalDescriptorHeap.h>
#include<Gear/Core/GlobalRootSignature.h>

class CommandList
{
public:

	CommandList(const D3D12_COMMAND_LIST_TYPE type);

	~CommandList();

	void reset() const;

	void setDescriptorHeap(DescriptorHeap* const resourceHeap, DescriptorHeap* const samplerHeap) const;

	void setGraphicsRootSignature(RootSignature* const rootSignature) const;

	void setComputeRootSignature(RootSignature* const rootSignature) const;

	ID3D12GraphicsCommandList6* get() const;

private:

	CommandAllocator* allocators[Graphics::FrameBufferCount];

	ComPtr<ID3D12GraphicsCommandList6> commandList;

};

#endif // !_COMMANDLIST_H_
