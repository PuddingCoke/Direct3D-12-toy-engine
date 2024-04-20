#pragma once

#include<Gear/Core/GraphicsContext.h>
#include<Gear/Core/ResourceManager.h>

class Material
{
public:

	Material(ResourceManager* const resManager, const std::string& diffusePath, const std::string& specularPath, std::string& normalPath)
	{
		diffuse = resManager->createTextureRenderTarget(diffusePath, true);

		specular = resManager->createTextureRenderTarget(specularPath, true);

		normal = resManager->createTextureRenderTarget(normalPath, true);
	}

	~Material()
	{
		delete diffuse;
		delete specular;
		delete normal;
	}

	//use first 3 slots of pixel constants
	void bind(GraphicsContext* const context)
	{
		context->setPSConstants({ diffuse->getAllSRVIndex(),specular->getAllSRVIndex(),normal->getAllSRVIndex() }, 0);
	}

private:

	TextureRenderTarget* diffuse;

	TextureRenderTarget* specular;

	TextureRenderTarget* normal;

};