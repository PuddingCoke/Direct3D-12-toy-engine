#pragma once

#ifndef _SWAPTEXTURE_H_
#define _SWAPTEXTURE_H_

#include"TextureRenderView.h"

#include<functional>

class SwapTexture
{
private:

	TextureRenderView* texture0;

	TextureRenderView* texture1;

public:

	SwapTexture(const std::function<TextureRenderView* (void)>& factoryFunc);

	~SwapTexture();

	TextureRenderView* read() const;

	TextureRenderView* write() const;

	void swap();

	const uint32_t width;

	const uint32_t height;

};

#endif // !_SWAPTEXTURE_H_