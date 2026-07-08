#include<Gear/Core/D3D12Core/Fence.h>

namespace Gear::Core::D3D12Core
{
	Fence::Fence(const D3D12_FENCE_FLAGS flags) :
		fenceEvent(CreateEvent(nullptr, FALSE, FALSE, nullptr)), currentFenceValue(0ull)
	{
		GraphicsDevice::get()->CreateFence(currentFenceValue, flags, IID_PPV_ARGS(&fence));
	}

	Fence::~Fence()
	{
		if (fenceEvent)
		{
			CloseHandle(fenceEvent);
		}
	}

	ID3D12Fence* Fence::get() const
	{
		return fence.Get();
	}

	void Fence::increment()
	{
		currentFenceValue++;
	}

	void Fence::waitCurrentValue()
	{
		fence->SetEventOnCompletion(currentFenceValue, fenceEvent);

		WaitForSingleObjectEx(fenceEvent, INFINITE, FALSE);
	}

	void Fence::waitValue(const uint64_t value)
	{
		fence->SetEventOnCompletion(value, fenceEvent);

		WaitForSingleObjectEx(fenceEvent, INFINITE, FALSE);
	}

	void Fence::signal(ID3D12CommandQueue* const commandQueue)
	{
		increment();

		commandQueue->Signal(fence.Get(), currentFenceValue);
	}

	void Fence::wait(ID3D12CommandQueue* const commandQueue)
	{
		commandQueue->Wait(fence.Get(), currentFenceValue);
	}

	uint64_t Fence::getCurrentFenceValue() const
	{
		return currentFenceValue;
	}

	uint64_t Fence::getCompletedValue() const
	{
		return fence->GetCompletedValue();
	}
}
