﻿#pragma once

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

private:

	HWND hwnd;

	MSG msg;

};

#endif // !_WIN32FORM_H_