#pragma once

#ifndef _GRAPHICS_H_
#define _GRAPHICS_H_

#include<Gear/Core/GraphicsDevice.h>

using Microsoft::WRL::ComPtr;

class Graphics
{
public:

	static constexpr UINT FrameBufferCount = 3;

	static UINT getFrameIndex();

	static FLOAT getDeltaTime();

	static FLOAT getTimeElapsed();

	static UINT getWidth();

	static UINT getHeight();

	static FLOAT getAspectRatio();

private:

	friend class RenderEngine;

	friend class Gear;

	static UINT frameIndex;

	static UINT width;

	static UINT height;

	static FLOAT aspectRatio;

	static struct Time
	{
		FLOAT deltaTime;
		FLOAT timeElapsed;
		UINT uintSeed;
		FLOAT floatSeed;
	} time;

	static Graphics* instance;

};

#endif // !_GRAPHICS_H_