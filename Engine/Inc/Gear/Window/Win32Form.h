#pragma once

#ifndef _WIN32FORM_H_
#define _WIN32FORM_H_

#include<Windows.h>

#include<string>

class Win32Form
{
public:

	Win32Form() = delete;

	Win32Form(const Win32Form&) = delete;

	void operator=(const Win32Form&) = delete;

	Win32Form(const std::wstring& title, const uint32_t width, const uint32_t height, const DWORD windowStyle,
		LRESULT(*windowCallback)(HWND hwnd, uint32_t msg, WPARAM wParam, LPARAM lParam));

	bool pollEvents();

	HWND getHandle() const;

	static Win32Form* get();

	static constexpr DWORD normalWindowStyle = WS_CAPTION | WS_SYSMENU;

	static LRESULT CALLBACK windowCallback(HWND hWnd, uint32_t uMsg, WPARAM wParam, LPARAM lParam);

	static LRESULT CALLBACK encodeCallback(HWND hWnd, uint32_t uMsg, WPARAM wParam, LPARAM lParam);

private:

	LRESULT CALLBACK windowProc(HWND hWnd, uint32_t uMsg, WPARAM wParam, LPARAM lParam);

	LRESULT CALLBACK encodeProc(HWND hWnd, uint32_t uMsg, WPARAM wParam, LPARAM lParam);

	friend class Gear;

	static Win32Form* instance;

	HWND hWnd;

	MSG msg;

	HMENU hMenu;

};

#endif // !_WIN32FORM_H_