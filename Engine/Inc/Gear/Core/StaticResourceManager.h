#pragma once

#ifndef _STATICRESOURCEMANAGER_H_
#define _STATICRESOURCEMANAGER_H_

#include<Gear/Core/ResourceManager.h>

class StaticResourceManager :public ResourceManager
{
private:

	friend class RenderEngine;

	static StaticResourceManager* instance;

	static StaticResourceManager* get();

	StaticResourceManager();

	~StaticResourceManager();

};

#endif // !_STATICRESOURCEMANAGER_H_
