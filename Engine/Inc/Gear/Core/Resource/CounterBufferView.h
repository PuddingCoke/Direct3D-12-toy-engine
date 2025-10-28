#pragma once

#ifndef _CORE_RESOURCE_COUNTERBUFFERVIEW_H_
#define _CORE_RESOURCE_COUNTERBUFFERVIEW_H_

#include"D3D12Resource/Buffer.h"

#include"EngineResource.h"

namespace Core
{
	namespace Resource
	{
		class CounterBufferView :public EngineResource
		{
		public:

			CounterBufferView() = delete;

			CounterBufferView(const bool persistent);

			~CounterBufferView();

			D3D12Resource::ShaderResourceDesc getSRVIndex() const;

			D3D12Resource::ShaderResourceDesc getUAVIndex() const;

			D3D12Resource::ClearUAVDesc getClearUAVDesc() const;

			void copyDescriptors() override;

			D3D12Resource::Buffer* getBuffer() const;

		private:

			uint32_t srvIndex;

			uint32_t uavIndex;

			D3D12_GPU_DESCRIPTOR_HANDLE viewGPUHandle;

			D3D12_CPU_DESCRIPTOR_HANDLE viewCPUHandle;

			D3D12Resource::Buffer* buffer;
		};
	}
}

#endif // !_CORE_RESOURCE_COUNTERBUFFERVIEW_H_