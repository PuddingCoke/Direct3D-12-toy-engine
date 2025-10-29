﻿#pragma once

#ifndef _GEAR_CORE_RESOURCE_STATICCBUFFER_H_
#define _GEAR_CORE_RESOURCE_STATICCBUFFER_H_

#include<Gear/Core/Resource/D3D12Resource/UploadHeap.h>

#include"ImmutableCBuffer.h"

namespace Gear
{
	namespace Core
	{
		namespace Resource
		{
			class StaticCBuffer :public ImmutableCBuffer
			{
			public:

				StaticCBuffer(D3D12Resource::Buffer* const buffer, const uint32_t size, const bool persistent);

				~StaticCBuffer();

				struct UpdateStruct
				{
					D3D12Resource::Buffer* const buffer;
					D3D12Resource::UploadHeap* const uploadHeap;
				}getUpdateStruct(const void* const data, const uint64_t size);

			private:

				D3D12Resource::UploadHeap** uploadHeaps;

				void** dataPtrs;

			};
		}
	}
}

#endif // !_GEAR_CORE_RESOURCE_STATICCBUFFER_H_
