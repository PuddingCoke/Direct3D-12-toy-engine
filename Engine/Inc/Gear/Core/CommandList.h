#pragma once

#ifndef _COMMANDLIST_H_
#define _COMMANDLIST_H_

#include"CommandAllocator.h"
#include"Graphics.h"

class CommandList
{
public:

	CommandList(const D3D12_COMMAND_LIST_TYPE type);

	~CommandList();

	void Reset();

	ID3D12GraphicsCommandList7* get();

private:

	CommandAllocator* allocators;

	ComPtr<ID3D12GraphicsCommandList7> commandList;

};

#endif // !_COMMANDLIST_H_
