#pragma once

#ifndef _GEAR_UTILS_MATH_H_
#define _GEAR_UTILS_MATH_H_

#include<cmath>

#include<DirectXMath.h>

#include<DirectXPackedVector.h>

#include<type_traits>

namespace Gear::Utils::Math
{
	constexpr float halfPi = DirectX::XM_PIDIV2;

	constexpr float pi = DirectX::XM_PI;

	constexpr float twoPi = DirectX::XM_2PI;

	constexpr float radToDeg = 180.f / Math::pi;

	constexpr float degToRad = 1.f / radToDeg;

	template<typename T>
		requires std::is_same_v<T, float> || std::is_same_v<T, double>
	constexpr T lerp(const T x, const T y, const T s)
	{
		return x * (static_cast<T>(1) - s) + y * s;
	}

	template<typename T>
		requires std::is_same_v<T, float> || std::is_same_v<T, double> || std::is_same_v<T, int32_t> || std::is_same_v<T, int64_t> || std::is_same_v<T, uint32_t> || std::is_same_v<T, uint64_t>
	constexpr T clamp(const T value, const T min, const T max)
	{
		if (value < min)
		{
			return min;
		}
		else if (value > max)
		{
			return max;
		}
		else
		{
			return value;
		}
	}

	template<typename T>
		requires std::is_same_v<T, float> || std::is_same_v<T, double>
	constexpr T saturate(const T value)
	{
		return clamp(value, static_cast<T>(0), static_cast<T>(1));
	}

	template<typename T>
		requires std::is_same_v<T, uint32_t> || std::is_same_v<T, uint64_t>
	constexpr T ceil(const T a, const T b)
	{
		return (a + b - static_cast<T>(1)) / b;
	}

	template<typename T>
		requires std::is_same_v<T, uint32_t> || std::is_same_v<T, uint64_t>
	constexpr T align(const T a, const T b)
	{
		return ceil(a, b) * b;
	}

	//a是否拥有b？
	template<typename T>
		requires std::is_same_v<T, uint32_t> || std::is_same_v<T, uint64_t>
	constexpr bool bitFlagSubset(const T a, const T b)
	{
		return b && ((a & b) == b);
	}

	float gauss(const float sigma, const float x);

	float pow(const float x, const float y);

	uint32_t log2(const uint32_t x);
}

#endif // !_GEAR_UTILS_MATH_H_
