#pragma once

#ifndef _CORE_DYNAMICCBUFFERMANAGER_INTERNAL_H_
#define _CORE_DYNAMICCBUFFERMANAGER_INTERNAL_H_

#include<Gear/Core/D3D12Core/CommandList.h>

namespace Core
{
	namespace DynamicCBufferManager
	{
		namespace Internal
		{
			void initialize();

			void release();

			void recordCommands(D3D12Core::CommandList* const commandList);
		}
	}
}

#endif // !_CORE_DYNAMICCBUFFERMANAGER_INTERNAL_H_
