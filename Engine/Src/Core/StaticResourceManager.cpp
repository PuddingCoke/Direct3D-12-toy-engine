#include<Gear/Core/StaticResourceManager.h>

#include<Gear/Core/StaticEffect/BackBufferBlitEffect.h>

#include<Gear/Core/StaticEffect/HDRClampEffect.h>

#include<Gear/Core/StaticEffect/LatLongMapToCubeMapEffect.h>

StaticResourceManager* StaticResourceManager::instance = nullptr;

StaticResourceManager* StaticResourceManager::get()
{
	return instance;
}

StaticResourceManager::StaticResourceManager():
	ResourceManager()
{
	context->begin();

	BackBufferBlitEffect::instance = new BackBufferBlitEffect();

	HDRClampEffect::instance = new HDRClampEffect();

	LatLongMapToCubeMapEffect::instance = new LatLongMapToCubeMapEffect(this);
}

StaticResourceManager::~StaticResourceManager()
{
	if (BackBufferBlitEffect::instance)
	{
		delete BackBufferBlitEffect::instance;
	}

	if (HDRClampEffect::instance)
	{
		delete HDRClampEffect::instance;
	}

	if (LatLongMapToCubeMapEffect::instance)
	{
		delete LatLongMapToCubeMapEffect::instance;
	}
}
