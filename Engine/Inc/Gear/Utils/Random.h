#pragma once

#ifndef _RANDOM_H_
#define _RANDOM_H_

#include<cstdint>

namespace Random
{

	//mean 0 sd(sigma) 1
	float genGauss();

	float genFloat();

	double genDouble();

	int32_t genInt();

	uint32_t genUint();

}

#endif // !_RANDOM_H_