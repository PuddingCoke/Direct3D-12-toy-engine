#pragma once

#ifndef _SWAPTEXTURE_H_
#define _SWAPTEXTURE_H_

#include"TextureRenderTarget.h"

#include"EngineResource.h"

#include<functional>

class SwapTexture :public EngineResource
{
private:

	TextureRenderTarget* texture0;

	TextureRenderTarget* texture1;

public:

	SwapTexture(const std::function<TextureRenderTarget* (void)>& factoryFunc);

	~SwapTexture();

	TextureRenderTarget* read() const;

	TextureRenderTarget* write() const;

	void swap();

	const UINT width;

	const UINT height;

};

#endif // !_SWAPTEXTURE_H_
