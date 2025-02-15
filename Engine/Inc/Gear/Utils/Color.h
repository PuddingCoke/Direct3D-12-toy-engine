﻿#pragma once

#ifndef _COLOR_H_
#define _COLOR_H_

#include"Random.h"
#include"Math.h"

#include<DirectXColors.h>

struct Color
{
	float r;

	float g;

	float b;

	float a;

	int toInt() const;

	bool operator==(const Color& color) const;

	bool operator!=(const Color& color) const;

	operator const float* () const;

	static Color random();

	static Color hsvToRgb(const Color& c);

};

#endif // !_COLOR_H_