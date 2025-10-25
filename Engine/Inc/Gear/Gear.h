#pragma once

#ifndef _GEAR_H_
#define _GEAR_H_

#include<Gear/Utils/DeltaTimeEstimator.h>

#include<Gear/InitializationParam.h>

#include<Gear/Game.h>

class Gear
{
public:

	Gear(const Gear&) = delete;

	void operator=(const Gear&) = delete;

	static Gear* get();

	int32_t iniEngine(const InitializationParam& param, const int32_t argc, const wchar_t* argv[]);

	void iniGame(Game* const gamePtr);

	static void initialize();

	static void release();

private:

	static Gear* instance;

	void runRealTimeRender();

	void runVideoRender();

	void runWallpaper();

	void reportLiveObjects() const;

	Gear();

	~Gear();

	Game* game;

	//screenshot
	ReadbackHeap* backBufferHeap;

	InitializationParam::EngineUsage usage;

	union
	{
		InitializationParam::RealTimeRenderParam realTimeRender;

		InitializationParam::VideoRenderParam videoRender;
	};

	static constexpr Input::Keyboard::Key screenGrabKey = Input::Keyboard::F11;

};

#endif // !_GEAR_H_