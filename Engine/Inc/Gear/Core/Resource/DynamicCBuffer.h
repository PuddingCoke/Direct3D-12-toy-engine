#pragma once

#ifndef _DYNAMICCBUFFER_H_
#define _DYNAMICCBUFFER_H_

#include"ImmutableCBuffer.h"

/// <summary>
/// must update it once per frame
/// </summary>
class DynamicCBuffer :public ImmutableCBuffer
{
public:

	DynamicCBuffer(const uint32_t size);

	void update(const void* const data);

private:

	uint32_t regionIndex;

};

#endif // !_DYNAMICCBUFFER_H_
