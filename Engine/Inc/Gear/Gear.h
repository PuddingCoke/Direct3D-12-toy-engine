#pragma once

#ifndef _GEAR_H_
#define _GEAR_H_

#include<Gear/Core/RenderEngine.h>
#include<Gear/Core/Graphics.h>
#include<Gear/Window/Win32Form.h>
#include<Gear/Utils/Utils.h>
#include<Gear/Utils/Random.h>
#include<Gear/Input/Keyboard.h>
#include<Gear/Input/Mouse.h>
#include<Gear/Configuration.h>
#include<Gear/Game.h>
#include<Gear/Core/VideoEncoder/NvidiaEncoder.h>

#include<iostream>
#include<chrono>
#include<windowsx.h>
#include<dxgidebug.h>

class Gear
{
public:

	Gear(const Gear&) = delete;

	void operator=(const Gear&) = delete;

	static Gear* get();

	int iniEngine(const Configuration config, const int argc, const char* argv[]);

	void iniGame(Game* const gamePtr);

	static void initialize();

	static void release();

private:

	static Gear* instance;

	void runGame();

	void runEncode();

	void destroy();

	void reportLiveObjects();

	Gear();

	~Gear();

	Win32Form* winform;

	Game* game;

	Configuration::EngineUsage usage;

	void iniWindow(const std::wstring title, const UINT width, const UINT height);

	static constexpr DWORD normalWndStyle = WS_CAPTION | WS_SYSMENU;

	static LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	static LRESULT CALLBACK EncodeProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
};

#endif // !_GEAR_H_
