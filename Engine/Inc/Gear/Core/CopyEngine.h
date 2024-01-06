#pragma once

#ifndef _COPYENGINE_H_
#define _COPYENGINE_H_

#include<Gear/Core/DX/Resource/Resource.h>
#include<Gear/Core/CommandList.h>

#include<mutex>

class CopyEngine
{
public:

	CopyEngine(const CopyEngine&) = delete;

	void operator=(const CopyEngine&) = delete;

	static void pushResourceCopy(Resource* const dstRes, Resource* const srcRes);

private:

	friend class Gear;

	CopyEngine();

	~CopyEngine();

	static CopyEngine* instance;

	CommandList* commandList;

	std::mutex updateMutex;

};

#endif // !_COPYENGINE_H_
