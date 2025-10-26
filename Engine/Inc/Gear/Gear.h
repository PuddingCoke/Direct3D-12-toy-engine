﻿#pragma once

#ifndef _GEAR_H_
#define _GEAR_H_

#include<Gear/Utils/DeltaTimeEstimator.h>

#include<Gear/InitializationParam.h>

#include<Gear/Game.h>

namespace Gear
{

	int32_t iniEngine(const InitializationParam& param, const int32_t argc, const wchar_t* argv[]);

	void iniGame(Game* const gamePtr);

	void initialize();

	void release();

}

#endif // !_GEAR_H_