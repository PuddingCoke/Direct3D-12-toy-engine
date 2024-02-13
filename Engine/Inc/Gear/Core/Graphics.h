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

	static D3D12_CPU_DESCRIPTOR_HANDLE getBackBufferHandle();

private:

	friend class Gear;

	friend class RenderEngine;

	static std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> backBufferHandles;

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

};

#endif // !_GRAPHICS_H_