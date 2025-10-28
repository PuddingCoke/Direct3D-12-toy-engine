#pragma once

#ifndef _CORE_RESOURCE_ENGINERESOURCE_H_
#define _CORE_RESOURCE_ENGINERESOURCE_H_

#include<Gear/Core/GlobalDescriptorHeap.h>

#include"D3D12Resource/D3D12ResourceDesc.h"

namespace Core
{
	namespace Resource
	{
		class EngineResource
		{
		public:

			EngineResource() = delete;

			EngineResource(const bool persistent);

			virtual ~EngineResource();

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

#endif // !_CORE_RESOURCE_ENGINERESOURCE_H_