#include<Gear/Core/Graphics.h>

UINT Graphics::frameIndex = 0;

UINT Graphics::width = 0;

UINT Graphics::height = 0;

FLOAT Graphics::aspectRatio = 0.f;

Graphics::Time Graphics::time = { 1.f / 60.f,0.f,0u,0.f };

std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> Graphics::backBufferHandles;

DXGI_FORMAT Graphics::backBufferFormat = DXGI_FORMAT_UNKNOWN;

UINT Graphics::getFrameIndex()
{
	return frameIndex;
}

FLOAT Graphics::getDeltaTime()
{
	return time.deltaTime;
}

FLOAT Graphics::getTimeElapsed()
{
	return time.timeElapsed;
}

UINT Graphics::getWidth()
{
	return width;
}

UINT Graphics::getHeight()
{
	return height;
}

FLOAT Graphics::getAspectRatio()
{
	return aspectRatio;
}

D3D12_CPU_DESCRIPTOR_HANDLE Graphics::getBackBufferHandle()
{
	return backBufferHandles[Graphics::frameIndex];
}

DXGI_FORMAT Graphics::getBackBufferFormat()
{
	return backBufferFormat;
}
