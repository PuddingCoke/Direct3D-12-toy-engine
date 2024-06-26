﻿#pragma once

#ifndef _MATH_H_
#define _MATH_H_

#include<cmath>
#include<DirectXMath.h>
#include<DirectXPackedVector.h>

namespace Math
{
	constexpr float half_pi = DirectX::XM_PIDIV2;

	constexpr float pi = DirectX::XM_PI;

	constexpr float two_pi = DirectX::XM_2PI;

	constexpr float radToDeg = 180.f / Math::pi;

	constexpr float degToRad = 1.f / radToDeg;

	float lerp(const float x, const float y, const float s);

	float clamp(const float value, const float min, const float max);

	float saturate(const float value);

	float gauss(const float sigma, const float x);

}

#endif // !_MATH_H_