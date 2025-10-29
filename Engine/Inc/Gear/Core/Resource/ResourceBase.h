#pragma once

#ifndef _CORE_RESOURCE_RESOURCEBASE_H_
#define _CORE_RESOURCE_RESOURCEBASE_H_

#include<Gear/Core/GlobalDescriptorHeap.h>

#include"D3D12Resource/PipelineResourceDesc.h"

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

			//copy non shader visible resource heap to shader visible resource heap
			D3D12Core::DescriptorHandle getTransientDescriptorHandle() const;

			const bool persistent;

			D3D12_CPU_DESCRIPTOR_HANDLE srvUAVCBVHandleStart;

			uint32_t numSRVUAVCBVDescriptors;

		};
	}
}

#endif // !_CORE_RESOURCE_RESOURCEBASE_H_