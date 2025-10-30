#pragma once

#ifndef _GEAR_CORE_RESOURCE_RESOURCEBASE_H_
#define _GEAR_CORE_RESOURCE_RESOURCEBASE_H_

#include<Gear/Core/GlobalDescriptorHeap.h>

#include"D3D12Resource/PipelineResourceDesc.h"

namespace Gear
{
	namespace Core
	{
		namespace Resource
		{
			class ResourceBase
			{
			public:

				ResourceBase() = delete;

				ResourceBase(const bool persistent);

				virtual ~ResourceBase();

				virtual void copyDescriptors();

				bool getPersistent() const;

			protected:

				//把非着色器可见的资源描述符堆上的描述符拷贝到着色器可见的资源描述符堆上，并返回DescriptorHandle
				D3D12Core::DescriptorHandle getTransientDescriptorHandle() const;

				const bool persistent;

				D3D12_CPU_DESCRIPTOR_HANDLE srvUAVCBVHandleStart;

				uint32_t numSRVUAVCBVDescriptors;

			};
		}
	}
}

#endif // !_GEAR_CORE_RESOURCE_RESOURCEBASE_H_