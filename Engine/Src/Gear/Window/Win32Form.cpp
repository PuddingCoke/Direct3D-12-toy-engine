#include<Gear/Window/Win32Form.h>

Win32Form::Win32Form(const std::wstring& title, const uint32_t width, const uint32_t height, const DWORD windowStyle, LRESULT(*windowCallback)(HWND hwnd, uint32_t msg, WPARAM wParam, LPARAM lParam)):
	hwnd(nullptr), msg{}
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

	hwnd = CreateWindow(L"MyWindowClass", title.c_str(), windowStyle, CW_USEDEFAULT, CW_USEDEFAULT, 
		rect.right - rect.left, rect.bottom - rect.top, nullptr, nullptr, hInstance, nullptr);

	if (!hwnd)
	{
		throw "Failed when create window";
	}

	ShowWindow(hwnd, SW_SHOW);
}

bool Win32Form::pollEvents()
{
	if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);

		DispatchMessage(&msg);
	}
	
	return msg.message != WM_QUIT;
}

HWND Win32Form::getHandle() const
{
	return hwnd;
}
