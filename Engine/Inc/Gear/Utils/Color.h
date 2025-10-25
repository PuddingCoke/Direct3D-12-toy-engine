#pragma once

#ifndef _UTILS_COLOR_H_
#define _UTILS_COLOR_H_

#include<cstdint>

#include<DirectXColors.h>

namespace Utils
{
	struct Color
	{

		float r;

		float g;

		float b;

		float a;

		uint32_t toUint() const;

		bool operator==(const Color& color) const;

		bool operator!=(const Color& color) const;

		operator const float* () const;

		static Color random();

		static Color hsvToRgb(const Color& c);

	};
}

#endif // !_UTILS_COLOR_H_