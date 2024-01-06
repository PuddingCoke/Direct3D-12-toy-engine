#pragma once

#ifndef _WIN32FORM_H_
#define _WIN32FORM_H_

#include<Windows.h>
#include<string>

class Win32Form
{
public:

	Win32Form(const std::wstring& title, const UINT& width, const UINT& height, const DWORD& windowStyle,
		LRESULT(*windowCallback)(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam));

	bool pollEvents();

	const HWND& getHandle() const;

private:

	HWND hwnd;

	MSG msg;

};

#endif // !_WIN32FORM_H_