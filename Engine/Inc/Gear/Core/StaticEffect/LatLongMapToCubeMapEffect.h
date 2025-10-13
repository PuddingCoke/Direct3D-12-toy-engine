#pragma once

#ifndef _LATLONGMAPTOCUBEMAPEFFECT_H_
#define _LATLONGMAPTOCUBEMAPEFFECT_H_

#include"StaticEffectHeader.h"

//this effect convert equirectangular map to cube map
class LatLongMapToCubeMapEffect
{
public:

	LatLongMapToCubeMapEffect(const LatLongMapToCubeMapEffect&) = delete;

	void operator=(const LatLongMapToCubeMapEffect&) = delete;

	static LatLongMapToCubeMapEffect* get();

	void process(GraphicsContext* const context, TextureRenderView* const inputTexture, TextureRenderView* const outputTexture) const;

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

	ImmutableCBuffer* matricesBuffer;

};

#endif // !_LATLONGMAPTOCUBEMAPEFFECT_H_
