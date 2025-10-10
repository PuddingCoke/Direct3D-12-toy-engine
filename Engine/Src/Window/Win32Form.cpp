#include<Gear/Window/Win32Form.h>

#include<Gear/Utils/Logger.h>

#include<Gear/Core/Graphics.h> 

#include<Gear/Input/Keyboard.h>

#include<Gear/Input/Mouse.h>

#include<ImGUI/imgui.h>

#include<ImGUI/imgui_impl_win32.h>

Win32Form* Win32Form::instance = nullptr;

Win32Form::Win32Form(const std::wstring& title, const uint32_t width, const uint32_t height, const DWORD windowStyle, LRESULT(*windowCallback)(HWND hwnd, uint32_t msg, WPARAM wParam, LPARAM lParam)) :
	hWnd(nullptr), msg{}, hMenu(nullptr)
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

	hWnd = CreateWindow(L"MyWindowClass", title.c_str(), windowStyle, CW_USEDEFAULT, CW_USEDEFAULT,
		rect.right - rect.left, rect.bottom - rect.top, nullptr, nullptr, hInstance, nullptr);

	if (!hWnd)
	{
		LOGERROR(L"create window failed");
	}

	ShowWindow(hWnd, SW_SHOW);
}

bool Win32Form::pollEvents()
{
	Mouse::resetDeltaInfo();

	Keyboard::resetOnKeyDownMap();

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

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, uint32_t msg, WPARAM wParam, LPARAM lParam);

LRESULT Win32Form::windowProc(HWND hWnd, uint32_t uMsg, WPARAM wParam, LPARAM lParam)
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

LRESULT Win32Form::encodeProc(HWND hWnd, uint32_t uMsg, WPARAM wParam, LPARAM lParam)
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
