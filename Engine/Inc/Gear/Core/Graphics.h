#pragma once

#ifndef _GRAPHICS_H_
#define _GRAPHICS_H_

#include<Gear/Core/GraphicsDevice.h>

using Microsoft::WRL::ComPtr;

class Graphics
{
public:

	static UINT getFrameBufferCount();

	static UINT getFrameIndex();

	static float getDeltaTime();

	static float getTimeElapsed();

	static UINT getWidth();

	static UINT getHeight();

	static float getAspectRatio();

	static constexpr DXGI_FORMAT BackBufferFormat = DXGI_FORMAT_B8G8R8A8_UNORM;

private:

	friend class RenderEngine;

	static UINT frameBufferCount;

	static UINT frameIndex;

	static UINT width;

	static UINT height;

	static float aspectRatio;

	static float deltaTime;

	static float timeElapsed;

};

#endif // !_GRAPHICS_H_