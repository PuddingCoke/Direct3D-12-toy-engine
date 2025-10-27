#include<Gear/Utils/Math.h>

float Utils::Math::lerp(const float x, const float y, const float s)
{
	return x * (1.f - s) + y * s;
}

float Utils::Math::clamp(const float value, const float min, const float max)
{
	if (value < min)
	{
		return min;
	}
	else if (value > max)
	{
		return max;
	}

	return value;
}

float Utils::Math::saturate(const float value)
{
	return clamp(value, 0.f, 1.f);
}

float Utils::Math::gauss(const float sigma, const float x)
{
	return 1.f / (sigma * 2.506628274631000502415765284811f) * exp(-0.5f * (x / sigma) * (x / sigma));
}

uint32_t Utils::Math::log2(const uint32_t x)
{
	unsigned long y;

	_BitScanReverse(&y, x);

	return y;
}
