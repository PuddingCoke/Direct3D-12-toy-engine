#pragma once

#ifndef _GEAR_H_
#define _GEAR_H_

#include<Gear/Core/RenderEngine.h>

#include<Gear/Window/Win32Form.h>

#include<Gear/Utils/DeltaTimeEstimator.h>

#include<Gear/InitializationParam.h>

#include<Gear/Game.h>

class Gear
{
public:

	Gear(const Gear&) = delete;

	void operator=(const Gear&) = delete;

	static Gear* get();

	int iniEngine(const InitializationParam& param, const int argc, const wchar_t* argv[]);

	void iniGame(Game* const gamePtr);

	static void initialize();

	static void release();

private:

	static Gear* instance;

	void runRealTimeRender();

	void runVideoRender();

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

	DeltaTimeEstimator dtEstimator;

	static constexpr Keyboard::Key screenGrabKey = Keyboard::F11;

};

#endif // !_GEAR_H_