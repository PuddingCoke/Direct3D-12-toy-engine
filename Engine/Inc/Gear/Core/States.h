#pragma once

#ifndef _STATES_H_
#define _STATES_H_

#include<Gear/Core/GraphicsDevice.h>

class States
{
public:

	States() = delete;

	States(const States&) = delete;

	void operator=(const States&) = delete;

	static D3D12_BLEND_DESC defBlendDesc;

	static D3D12_BLEND_DESC addtiveBlendDesc;

	static D3D12_RASTERIZER_DESC rasterShadow;

	static D3D12_RASTERIZER_DESC rasterCullBack;

	static D3D12_RASTERIZER_DESC rasterCullFront;

	static D3D12_RASTERIZER_DESC rasterCullNone;

	static D3D12_RASTERIZER_DESC rasterWireFrame;

	static D3D12_DEPTH_STENCIL_DESC depthLessEqual;

	static D3D12_DEPTH_STENCIL_DESC depthLess;

private:

	friend class RenderEngine;

	static void initialize();

};

#endif // !_STATES_H_