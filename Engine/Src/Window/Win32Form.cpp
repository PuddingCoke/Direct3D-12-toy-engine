#include<Gear/Window/Win32Form.h>

#include<Gear/Utils/Logger.h>

#include<Gear/Core/Graphics.h> 

#include<Gear/Input/Keyboard.h>

#include<Gear/Input/Mouse.h>

#include<ImGUI/imgui.h>

#include<ImGUI/imgui_impl_win32.h>

#define WM_TRAYICON WM_USER

#define EXITUID 0

Win32Form* Win32Form::instance = nullptr;

Win32Form::Win32Form(const std::wstring& title, const uint32_t startX, const uint32_t startY, const uint32_t width, const uint32_t height, const DWORD windowStyle, LRESULT(*windowCallback)(HWND hwnd, uint32_t msg, WPARAM wParam, LPARAM lParam)) :
	hWnd(nullptr), iniTrayIcon(windowCallback == wallpaperCallBack), hMenu(nullptr), nid{}, mouseHook(nullptr)
{
	SetProcessDPIAware();

	const HINSTANCE hInstance = GetModuleHandle(0);

	WNDCLASSEX wcex = {};
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = reinterpret_cast<HBRUSH>(GetStockObject(NULL_BRUSH));
	wcex.hIcon = LoadIcon(0, IDI_APPLICATION);
	wcex.hIconSm = LoadIcon(0, IDI_APPLICATION);
	wcex.lpszClassName = L"MyWindowClass";
	wcex.hInstance = hInstance;
	wcex.lpfnWndProc = windowCallback;

	RegisterClassEx(&wcex);

	RECT rect = { 0,0,static_cast<LONG>(width),static_cast<LONG>(height) };

	AdjustWindowRect(&rect, windowStyle, false);

	hWnd = CreateWindow(L"MyWindowClass", title.c_str(), windowStyle, startX, startY,
		rect.right - rect.left, rect.bottom - rect.top, nullptr, nullptr, hInstance, nullptr);

	if (!hWnd)
	{
		LOGERROR(L"create window failed");
	}

	ShowWindow(hWnd, SW_SHOW);

	if (iniTrayIcon)
	{
		nid.cbSize = sizeof(NOTIFYICONDATA);
		nid.hWnd = hWnd;
		nid.uID = 0;
		nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
		nid.uCallbackMessage = WM_TRAYICON;
		nid.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
		wcscpy_s(nid.szTip, L"动态壁纸");

		Shell_NotifyIcon(NIM_ADD, &nid);

		hMenu = CreatePopupMenu();

		AppendMenu(hMenu, MF_STRING, EXITUID, L"退出程序");

		mouseHook = SetWindowsHookEx(WH_MOUSE_LL, mouseHookCallback, nullptr, 0);
	}
}

Win32Form::~Win32Form()
{
	if (iniTrayIcon)
	{
		UnhookWindowsHookEx(mouseHook);

		Shell_NotifyIcon(NIM_DELETE, &nid);

		DestroyMenu(hMenu);
	}

	DestroyWindow(hWnd);
}

bool Win32Form::pollEvents()
{
	Mouse::resetDeltaInfo();

	Keyboard::resetOnKeyDownMap();

	MSG msg = {};

	if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);

		DispatchMessage(&msg);
	}

	return msg.message != WM_QUIT;
}

HWND Win32Form::getHandle() const
{
	return hWnd;
}

Win32Form* Win32Form::get()
{
	return instance;
}

LRESULT Win32Form::windowCallback(HWND hWnd, uint32_t uMsg, WPARAM wParam, LPARAM lParam)
{
	return instance->windowProc(hWnd, uMsg, wParam, lParam);
}

LRESULT Win32Form::encodeCallback(HWND hWnd, uint32_t uMsg, WPARAM wParam, LPARAM lParam)
{
	return instance->encodeProc(hWnd, uMsg, wParam, lParam);
}

LRESULT Win32Form::wallpaperCallBack(HWND hWnd, uint32_t uMsg, WPARAM wParam, LPARAM lParam)
{
	return instance->wallpaperProc(hWnd, uMsg, wParam, lParam);
}

LRESULT Win32Form::mouseHookCallback(int nCode, WPARAM wParam, LPARAM lParam)
{
	return instance->mouseHookProc(nCode, wParam, lParam);
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, uint32_t uMsg, WPARAM wParam, LPARAM lParam);

LRESULT Win32Form::windowProc(HWND hWnd, uint32_t uMsg, WPARAM wParam, LPARAM lParam) const
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam))
		return true;

	switch (uMsg)
	{
	case WM_PAINT:
	{
		PAINTSTRUCT ps;

		BeginPaint(hWnd, &ps);

		EndPaint(hWnd, &ps);
	}
	break;

	case WM_MOUSEMOVE:
		Mouse::move(static_cast<float>(LOWORD(lParam)), static_cast<float>(Graphics::getHeight()) - static_cast<float>(HIWORD(lParam)));
		break;

	case WM_LBUTTONDOWN:
		Mouse::pressLeft();
		break;

	case WM_RBUTTONDOWN:
		Mouse::pressRight();
		break;

	case WM_LBUTTONUP:
		Mouse::releaseLeft();
		break;

	case WM_RBUTTONUP:
		Mouse::releaseRight();
		break;

	case WM_MOUSEWHEEL:
		Mouse::scroll(GET_WHEEL_DELTA_WPARAM(wParam) / 120.f);
		break;

	case WM_KEYDOWN:
		if ((HIWORD(lParam) & KF_REPEAT) == 0)
		{
			Keyboard::pressKey(static_cast<Keyboard::Key>(wParam));
		}
		break;

	case WM_KEYUP:
		Keyboard::releaseKey(static_cast<Keyboard::Key>(wParam));
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}

	return 0;
}

LRESULT Win32Form::encodeProc(HWND hWnd, uint32_t uMsg, WPARAM wParam, LPARAM lParam) const
{
	switch (uMsg)
	{
	case WM_PAINT:
	{
		PAINTSTRUCT ps;

		BeginPaint(hWnd, &ps);

		EndPaint(hWnd, &ps);
	}
	break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}

	return 0;
}

LRESULT Win32Form::wallpaperProc(HWND hWnd, uint32_t uMsg, WPARAM wParam, LPARAM lParam) const
{
	switch (uMsg)
	{
	case WM_PAINT:
	{
		PAINTSTRUCT ps;

		BeginPaint(hWnd, &ps);

		EndPaint(hWnd, &ps);
	}
	break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_TRAYICON:
		if (LOWORD(lParam) == WM_RBUTTONUP)
		{
			POINT pt;

			GetCursorPos(&pt);

			TrackPopupMenu(hMenu, TPM_RIGHTBUTTON, pt.x, pt.y, 0, hWnd, nullptr);
		}
		break;

	case WM_COMMAND:
		if (LOWORD(wParam) == EXITUID)
		{
			PostQuitMessage(0);
		}
		break;

	default:
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}

	return 0;
}

LRESULT Win32Form::mouseHookProc(int nCode, WPARAM wParam, LPARAM lParam) const
{
	if (nCode == HC_ACTION)
	{
		MSLLHOOKSTRUCT* pMouseStruct = (MSLLHOOKSTRUCT*)lParam;

		switch (wParam)
		{
		case WM_MOUSEMOVE:
			Mouse::move(static_cast<float>(pMouseStruct->pt.x), static_cast<float>(Graphics::getHeight()) - static_cast<float>(pMouseStruct->pt.y));
			break;

		case WM_LBUTTONDOWN:
			Mouse::pressLeft();
			break;

		case WM_RBUTTONDOWN:
			Mouse::pressRight();
			break;

		case WM_LBUTTONUP:
			Mouse::releaseLeft();
			break;

		case WM_RBUTTONUP:
			Mouse::releaseRight();
			break;

		case WM_MOUSEWHEEL:
			Mouse::scroll(GET_WHEEL_DELTA_WPARAM(wParam) / 120.f);
			break;
		}

	}

	return CallNextHookEx(mouseHook, nCode, wParam, lParam);
}
