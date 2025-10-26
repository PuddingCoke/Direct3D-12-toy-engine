#pragma once

#ifndef _CORE_DYNAMICCBUFFERMANAGER_INTERNAL_H_
#define _CORE_DYNAMICCBUFFERMANAGER_INTERNAL_H_

#include<Gear/Core/CommandList.h>

namespace Core
{
	namespace DynamicCBufferManager
	{
		namespace Internal
		{
			void initialize();

			void release();

			void recordCommands(CommandList* const commandList);
		}
	}
}

#endif // !_CORE_DYNAMICCBUFFERMANAGER_INTERNAL_H_
