﻿#pragma once

#ifndef _GEAR_CORE_RESOURCE_IMMUTABLECBUFFER_H_
#define _GEAR_CORE_RESOURCE_IMMUTABLECBUFFER_H_

#include"D3D12Resource/Buffer.h"

#include"ResourceBase.h"

namespace Gear
{
	namespace Core
	{
		namespace Resource
		{
			class ImmutableCBuffer :public ResourceBase
			{
			public:

				ImmutableCBuffer() = delete;

				ImmutableCBuffer(const ImmutableCBuffer&) = delete;

				void operator=(const ImmutableCBuffer&) = delete;

				ImmutableCBuffer(D3D12Resource::Buffer* const buffer, const uint32_t size, const bool persistent);

				~ImmutableCBuffer();

				D3D12Resource::ShaderResourceDesc getBufferIndex() const;

				D3D12_GPU_VIRTUAL_ADDRESS getGPUAddress() const;

				D3D12Resource::Buffer* getBuffer() const;

				void copyDescriptors() override;

			protected:

				D3D12_GPU_VIRTUAL_ADDRESS gpuAddress;

				uint32_t bufferIndex;

				D3D12Resource::Buffer* buffer;

			};
		}
	}
}

#endif // !_GEAR_CORE_RESOURCE_IMMUTABLECBUFFER_H_