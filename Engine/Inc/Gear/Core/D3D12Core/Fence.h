#pragma once

#ifndef _GEAR_CORE_D3D12CORE_FENCE_H_
#define _GEAR_CORE_D3D12CORE_FENCE_H_

#include<Gear/Core/GraphicsDevice.h>

namespace Gear::Core::D3D12Core
{
	CREATESAFETYPE(Fence);

	class Fence
	{
	public:

		Fence(const D3D12_FENCE_FLAGS flags = D3D12_FENCE_FLAG_NONE);

		~Fence();

		ID3D12Fence* get() const;

		void increment();

		//CPU等待
		void waitCurrentValue();

		//CPU等待
		void waitValue(const uint64_t value);

		void signal(ID3D12CommandQueue* const commandQueue);

		void wait(ID3D12CommandQueue* const commandQueue);

		uint64_t getCurrentFenceValue() const;

		uint64_t getCompletedValue() const;

	private:

		ComPtr<ID3D12Fence> fence;

		HANDLE fenceEvent;

		uint64_t currentFenceValue;

	};
}

#endif // !_GEAR_CORE_D3D12CORE_FENCE_H_
