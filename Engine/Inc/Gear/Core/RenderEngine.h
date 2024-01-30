#pragma once

#ifndef _RENDERENGINE_H_
#define _RENDERENGINE_H_

#include<Gear/Core/RenderPass.h>

class RenderEngine
{
public:

	void submitRenderPass(RenderPass* const pass);

private:

	friend class Gear;

	ComPtr<ID3D12CommandQueue> commandQueue;

};

#endif // !_RENDERENGINE_H_

