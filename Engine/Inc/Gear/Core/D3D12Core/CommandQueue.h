#pragma once

#ifndef _GEAR_CORE_D3D12CORE_COMMANDQUEUE_H_
#define _GEAR_CORE_D3D12CORE_COMMANDQUEUE_H_

#include<Gear/Utils/StaticVector.h>

#include"CommandList.h"

#include"Fence.h"

namespace Gear::Core::D3D12Core
{
	CREATESAFETYPE(CommandQueue);

	class CommandQueue
	{
	public:

		CommandQueue(const D3D12_COMMAND_LIST_TYPE type);

		virtual ~CommandQueue();

		ID3D12CommandQueue* get() const;

		void setName(const wchar_t* const name);

		void setPrepareCommandList(CommandList* const commandList);

		void waitDestroyable();

		void waitFrameCPUReusable();

		void waitFrameGPUComplete();

		void begin();

		void submitCommandList(D3D12Core::CommandList* const commandList);

		void processCommandLists();

		D3D12Core::CommandList* getLastUsableCommandList() const;

	private:

		//仅内部使用，用于N重缓冲
		void signal();

		ComPtr<ID3D12CommandQueue> commandQueue;

		const D3D12_COMMAND_LIST_TYPE commandQueueType;

		FencePtr fence;

		UniquePtr<uint64_t[]> fenceValues;

		static constexpr uint64_t recordCommandListsLength = 32ull;

		Utils::StaticVector<CommandList*, recordCommandListsLength> recordCommandLists;

		Utils::StaticVector<ID3D12CommandList*, recordCommandListsLength> id3d12CommandLists;

		std::mutex submitCommandListMutex;

		std::vector<D3D12_RESOURCE_BARRIER> resourceBarriers;

		CommandList* prepareCommandList;

		D3D12Core::CommandList* lastUsableCommandList;

	};
}

#endif // !_GEAR_CORE_D3D12CORE_COMMANDQUEUE_H_
