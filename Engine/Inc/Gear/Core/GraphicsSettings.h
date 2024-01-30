#pragma once

#ifndef _GRAPHICSSETTINGS_H_
#define _GRAPHICSSETTINGS_H_

#include<Windows.h>

class GraphicsSettings
{
public:

	static constexpr UINT FrameBufferCount = 3;

	static UINT getFrameIndex();

private:

	friend class Gear;

	static UINT frameIndex;

};

#endif // !_GRAPHICSSETTINGS_H_