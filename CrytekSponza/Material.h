#pragma once

#include<Gear/Core/GraphicsContext.h>
#include<Gear/Core/ResourceManager.h>

class Material
{
public:

	Material(ResourceManager* const resManager, const std::string& diffusePath, const std::string& specularPath, std::string& normalPath)
	{
		const std::wstring wDiffusePath = std::wstring(diffusePath.begin(), diffusePath.end());

		const std::wstring wSpecularPath = std::wstring(specularPath.begin(), specularPath.end());

		const std::wstring wNormalPath = std::wstring(normalPath.begin(), normalPath.end());

		diffuse = resManager->createTextureRenderView(wDiffusePath, true);

		specular = resManager->createTextureRenderView(wSpecularPath, true);

		normal = resManager->createTextureRenderView(wNormalPath, true);
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

	TextureRenderView* diffuse;

	TextureRenderView* specular;

	TextureRenderView* normal;

};