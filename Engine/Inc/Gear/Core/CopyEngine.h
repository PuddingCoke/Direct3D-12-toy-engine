#pragma once

#ifndef _COPYENGINE_H_
#define _COPYENGINE_H_

#include<Gear/Core/DX/Resource/Resource.h>
#include<Gear/Core/DX/Resource/Buffer.h>
#include<Gear/Core/CommandList.h>

#include<mutex>

//dynamic resource update
//executed at begining
class CopyEngine
{
public:

	CopyEngine(const CopyEngine&) = delete;

	void operator=(const CopyEngine&) = delete;

	static void pushResourceCopy(Resource* const dstRes, Resource* const srcRes);

	static void pushBufferUpdate(Buffer* const buffer, const void* const data, const UINT dataSize);

private:

	friend class Gear;

	CopyEngine();

	~CopyEngine();

	static CopyEngine* instance;

	CommandList* commandList;

	std::mutex updateMutex;

};

#endif // !_COPYENGINE_H_
