#pragma once

#ifndef _GEAR_H_
#define _GEAR_H_

#include<Gear/Core/RenderEngine.h>

#include<Gear/Window/Win32Form.h>

#include<Gear/Utils/DeltaTimeEstimator.h>

#include<Gear/Configuration.h>

#include<Gear/Game.h>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, uint32_t msg, WPARAM wParam, LPARAM lParam);

class Gear
{
public:

	Gear(const Gear&) = delete;

	void operator=(const Gear&) = delete;

	static Gear* get();

	int iniEngine(const Configuration& config, const int argc, const char* argv[]);

	void iniGame(Game* const gamePtr);

	static void initialize();

	static void release();

private:

	static Gear* instance;

	void runGame();

	void runEncode();

	void reportLiveObjects() const;

	Gear();

	~Gear();

	Win32Form* winform;

	Game* game;

	Configuration::EngineUsage usage;

	DeltaTimeEstimator dtEstimator;

	void iniWindow(const std::wstring& title, const uint32_t width, const uint32_t height);

	static constexpr DWORD normalWndStyle = WS_CAPTION | WS_SYSMENU;

	static LRESULT CALLBACK windowCallback(HWND hWnd, uint32_t uMsg, WPARAM wParam, LPARAM lParam);

	static LRESULT CALLBACK encodeCallback(HWND hWnd, uint32_t uMsg, WPARAM wParam, LPARAM lParam);
};

#endif // !_GEAR_H_