﻿#pragma once

#ifndef _GEAR_CORE_RESOURCE_DYNAMICCBUFFER_H_
#define _GEAR_CORE_RESOURCE_DYNAMICCBUFFER_H_

#include"ImmutableCBuffer.h"

namespace Gear
{
	namespace Core
	{
		namespace Resource
		{
			/// <summary>
			/// 如果需要使用，那么每帧必须都更新一次
			/// </summary>
			class DynamicCBuffer :public ImmutableCBuffer
			{
			public:

				DynamicCBuffer(const uint32_t size);

				//获取可用的数据指针
				void acquireDataPtr();

				//更新数据指针的内容
				void updateData(const void* const data);

				//先获取再更新数据指针的内容
				void simpleUpdate(const void* const data);

			private:

				const uint32_t regionIndex;

				void* dataPtr;

				uint64_t acquireFrameIndex;

				uint64_t updateFrameIndex;

			};
		}
	}
}

#endif // !_GEAR_CORE_RESOURCE_DYNAMICCBUFFER_H_
