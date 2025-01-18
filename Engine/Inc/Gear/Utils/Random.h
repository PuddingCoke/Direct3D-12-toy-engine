#pragma once

#ifndef _RANDOM_H_
#define _RANDOM_H_

#include<random>
#include<cstdlib>
#include<ctime>

class Random
{
public:

	Random() = delete;

	Random(const Random&) = delete;

	void operator=(const Random&) = delete;

	//mean 0 sd(sigma) 1
	static float genGauss();

	static float genFloat();

	static double genDouble();

	static int genInt();

	static uint32_t genUint();

private:

	static std::mt19937 randomEngine;

	static std::normal_distribution<float> normalDistribution;

	static std::uniform_real_distribution<float> floatDistribution;

	static std::uniform_real_distribution<double> doubleDistribution;

	static std::uniform_int_distribution<int> intDistribution;

	static std::uniform_int_distribution<uint32_t> uintDistribution;

};

#endif // !_RANDOM_H_