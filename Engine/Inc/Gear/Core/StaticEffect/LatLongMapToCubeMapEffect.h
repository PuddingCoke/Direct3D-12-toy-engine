#pragma once

#ifndef _LATLONGMAPTOCUBEMAPEFFECT_H_
#define _LATLONGMAPTOCUBEMAPEFFECT_H_

#include<Gear/Core/ResourceManager.h>

#include<Gear/Core/Shader.h>

#include<Gear/Core/PipelineState.h>

//this effect convert equirectangular map to cube map
class LatLongMapToCubeMapEffect
{
public:

	static LatLongMapToCubeMapEffect* get();

	void process(GraphicsContext* const context, TextureRenderView* const inputTexture, TextureRenderView* const outputTexture);

private:

	friend class StaticResourceManager;

	static LatLongMapToCubeMapEffect* instance;

	LatLongMapToCubeMapEffect(ResourceManager* const resManager);

	~LatLongMapToCubeMapEffect();

	Shader* const equirectangularVS;

	Shader* const equirectangularPS;

	ComPtr<ID3D12PipelineState> equirectangularR8State;

	ComPtr<ID3D12PipelineState> equirectangularR16State;

	ComPtr<ID3D12PipelineState> equirectangularR32State;

	ConstantBuffer* matricesBuffer;

};

#endif // !_LATLONGMAPTOCUBEMAPEFFECT_H_
