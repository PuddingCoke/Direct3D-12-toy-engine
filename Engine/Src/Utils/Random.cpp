#include<Gear/Utils/Random.h>

std::mt19937 Random::randomEngine = std::mt19937(static_cast<uint32_t>(time(nullptr)));

std::normal_distribution<float> Random::normalDistribution = std::normal_distribution<float>(0.f, 1.f);

std::uniform_real_distribution<float> Random::floatDistribution = std::uniform_real_distribution<float>(0.f, 1.f);

std::uniform_real_distribution<double> Random::doubleDistribution = std::uniform_real_distribution<double>(0.0, 1.0);

std::uniform_int_distribution<int> Random::intDistribution;

std::uniform_int_distribution<uint32_t> Random::uintDistribution;

float Random::genGauss()
{
	return normalDistribution(randomEngine);
}

float Random::genFloat()
{
	return floatDistribution(randomEngine);
}

double Random::genDouble()
{
	return doubleDistribution(randomEngine);
}

int Random::genInt()
{
	return intDistribution(randomEngine);
}

uint32_t Random::genUint()
{
	return uintDistribution(randomEngine);
}
