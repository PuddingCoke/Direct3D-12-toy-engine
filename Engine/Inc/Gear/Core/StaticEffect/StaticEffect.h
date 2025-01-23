#pragma once

#ifndef _STATICEFFECT_H_
#define _STATICEFFECT_H_

#include"HDRClampEffect.h"

class StaticEffect
{
public:

	StaticEffect() = delete;

	~StaticEffect() = delete;

	StaticEffect(StaticEffect&) = delete;

	void operator=(StaticEffect&) = delete;

private:

	friend class RenderEngine;

	static void initializeStaticEffects();

	static void releaseStaticEffects();

};

#endif // !_STATICEFFECT_H_
