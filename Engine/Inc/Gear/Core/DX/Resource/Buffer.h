#pragma once

#ifndef _BUFFER_H_
#define _BUFFER_H_

#include<Gear/Core/GraphicsDevice.h>

#include"UploadHeap.h"

enum BufferUsage
{
	STATIC,
	DYNAMIC
};

class Buffer:public Resource
{
public:

	Buffer(const UINT size,const BufferUsage usage);

	void update(const void* const data, const UINT dataSize);

	virtual ~Buffer();

private: 

	UploadHeap* uploadHeaps;

	UINT uploadHeapIndex;

};

#endif // !_BUFFER_H_

