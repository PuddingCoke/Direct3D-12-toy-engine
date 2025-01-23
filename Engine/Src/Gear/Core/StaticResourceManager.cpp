#include<Gear/Core/StaticResourceManager.h>

StaticResourceManager* StaticResourceManager::instance = nullptr;

StaticResourceManager* StaticResourceManager::get()
{
	return instance;
}

StaticResourceManager::StaticResourceManager():
	ResourceManager()
{
	context->begin();

	HDRClampEffect::instance = new HDRClampEffect();

	LatLongMapToCubeMapEffect::instance = new LatLongMapToCubeMapEffect(this);
}

StaticResourceManager::~StaticResourceManager()
{
	if (HDRClampEffect::instance)
	{
		delete HDRClampEffect::instance;
	}

	if (LatLongMapToCubeMapEffect::instance)
	{
		delete LatLongMapToCubeMapEffect::instance;
	}
}
