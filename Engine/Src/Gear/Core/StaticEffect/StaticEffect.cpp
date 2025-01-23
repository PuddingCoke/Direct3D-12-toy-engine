#include<Gear/Core/StaticEffect/StaticEffect.h>

void StaticEffect::initializeStaticEffects()
{
	HDRClampEffect::initialize();
}

void StaticEffect::releaseStaticEffects()
{
	HDRClampEffect::release();
}
