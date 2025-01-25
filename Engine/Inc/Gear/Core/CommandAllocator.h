#pragma once

#ifndef _COMMANDALLOCATOR_H_
#define _COMMANDALLOCATOR_H_

#include<Gear/Core/GraphicsDevice.h>

class CommandAllocator
{
public:

	CommandAllocator() = delete;

	CommandAllocator(const CommandAllocator&) = delete;

	void operator=(const CommandAllocator&) = delete;

	CommandAllocator(const D3D12_COMMAND_LIST_TYPE type);

	~CommandAllocator();

	void reset() const;

	ID3D12CommandAllocator* get() const;

private:

	ComPtr<ID3D12CommandAllocator> commandAllocator;

};

#endif // !_COMMANDALLOCATOR_H_