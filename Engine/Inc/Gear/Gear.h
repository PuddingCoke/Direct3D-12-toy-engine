#pragma once

#ifndef _GEAR_H_
#define _GEAR_H_

#include<Gear/Core/RenderEngine.h>

#include<Gear/Window/Win32Form.h>

#include<Gear/Utils/DeltaTimeEstimator.h>

#include<Gear/InitializationParam.h>

#include<Gear/Game.h>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, uint32_t msg, WPARAM wParam, LPARAM lParam);

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

	Win32Form* winform;

	Game* game;

	InitializationParam::EngineUsage usage;

	union
	{
		InitializationParam::RealTimeRenderParam realTimeRender;

		InitializationParam::VideoRenderParam videoRender;
	};

	DeltaTimeEstimator dtEstimator;

	static constexpr DWORD normalWndStyle = WS_CAPTION | WS_SYSMENU;

	static LRESULT CALLBACK windowCallback(HWND hWnd, uint32_t uMsg, WPARAM wParam, LPARAM lParam);

	static LRESULT CALLBACK encodeCallback(HWND hWnd, uint32_t uMsg, WPARAM wParam, LPARAM lParam);
};

#endif // !_GEAR_H_