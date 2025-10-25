#pragma once

#ifndef _UTILS_RANDOM_H_
#define _UTILS_RANDOM_H_

#include<cstdint>

namespace Utils
{
	namespace Random
	{
		//均值 0 标准差 1
		float genGauss();

		float genFloat();

		double genDouble();

		int32_t genInt();

		uint32_t genUint();

	}
}

#endif // !_UTILS_RANDOM_H_