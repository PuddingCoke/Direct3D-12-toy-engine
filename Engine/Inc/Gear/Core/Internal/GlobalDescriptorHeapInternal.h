#pragma once

#ifndef _GEAR_CORE_GLOBALDESCRIPTORHEAP_INTERNAL_H_
#define _GEAR_CORE_GLOBALDESCRIPTORHEAP_INTERNAL_H_

namespace Gear
{
	namespace Core
	{
		namespace GlobalDescriptorHeap
		{
			namespace Internal
			{
				void initializeGlobalDescriptorHeaps();

				void initializeLocalDescriptorHeaps();

				void releaseGlobalDescriptorHeaps();

				void releaseLocalDescriptorHeaps();

				constexpr uint32_t numResourceHeapDescriptors = 1000000u;

				constexpr uint32_t numStaticSRVDescriptors = 100000u;

				constexpr uint32_t numStaticCBVSRVUAVDescriptors = numStaticSRVDescriptors * 2u;

				constexpr uint32_t numStagingResourceDescriptors = 2000000u;

				constexpr uint32_t numRTVDescriptors = 500000u;

				constexpr uint32_t numStaticRTVDescriptors = 150000u;

			}
		}
	}
}

#endif // !_GEAR_CORE_GLOBALDESCRIPTORHEAP_INTERNAL_H_
