#include<Gear/Gear.h>

Gear* Gear::instance = nullptr;

Gear* Gear::get()
{
	return instance;
}

int Gear::iniEngine(const Configuration config, const int argc, const char* argv[])
{
	std::wcout.imbue(std::locale(""));

	{
		std::string exeRootPath = argv[0];

		std::cout << "[class Gear] executable path " << exeRootPath << "\n";

		Utils::exeRootPath = exeRootPath;
	}

	usage = config.usage;

	iniWindow(config.title, config.width, config.height);

	Graphics::width = config.width;
	Graphics::height = config.height;
	Graphics::aspectRatio = static_cast<float>(Graphics::width) / static_cast<float>(Graphics::height);

	RenderEngine::instance = new RenderEngine(winform->getHandle());

	std::cout << "[class Gear] resolution " << Graphics::width << "x" << Graphics::height << "\n";
	std::cout << "[class Gear] aspect ratio " << Graphics::aspectRatio << "\n";
	std::cout << "[class Gear] back buffer count " << Graphics::FrameBufferCount << "\n";

	return 0;
}

void Gear::iniGame(Game* const gamePtr)
{
	game = gamePtr;

	RenderEngine::get()->processCommandLists();

	RenderEngine::get()->waitForGPU();

	switch (usage)
	{
	default:
	case Configuration::EngineUsage::NORMAL:
		std::cout << "[class Gear] usage normal\n";
		runGame();
		break;
	case Configuration::EngineUsage::VIDEOPLAYBACK:
		std::cout << "[class Gear] usage video playback\n";
		break;
	}

	RenderEngine::get()->waitForGPU();

	destroy();
}

void Gear::initialize()
{
	Gear::instance = new Gear();
}

void Gear::release()
{
	delete Gear::instance;
}

void Gear::runGame()
{
	std::chrono::high_resolution_clock clock;

	while (winform->pollEvents())
	{
		const std::chrono::high_resolution_clock::time_point startPoint = clock.now();

		RenderEngine::get()->begin();

		game->update(Graphics::time.deltaTime);

		game->render();

		RenderEngine::get()->end();

		const std::chrono::high_resolution_clock::time_point endPoint = clock.now();

		Graphics::time.deltaTime = std::chrono::duration_cast<std::chrono::milliseconds>(endPoint - startPoint).count() / 1000.f;

		Graphics::time.timeElapsed += Graphics::time.deltaTime;

		Graphics::time.uintSeed = Random::Uint();

		Graphics::time.floatSeed = Random::Float();
	}

}

void Gear::destroy()
{
	delete game;

	delete winform;

	delete RenderEngine::instance;

	reportLiveObjects();

	if (GraphicsDevice::instance)
	{
		delete GraphicsDevice::instance;
	}
}

void Gear::reportLiveObjects()
{
	ComPtr<IDXGIDebug1> dxgiDebug;

	if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&dxgiDebug))))
	{
		dxgiDebug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_FLAGS(DXGI_DEBUG_RLO_DETAIL | DXGI_DEBUG_RLO_IGNORE_INTERNAL));
	}
}

Gear::Gear() :
	game(nullptr), winform(nullptr), usage(Configuration::EngineUsage::NORMAL)
{

}

Gear::~Gear()
{

}

void Gear::iniWindow(const std::wstring& title, const UINT& width, const UINT& height)
{
	switch (usage)
	{
	case Configuration::EngineUsage::NORMAL:
		winform = new Win32Form(title, width, height, normalWndStyle, Gear::WindowProc);
		break;

	case Configuration::EngineUsage::VIDEOPLAYBACK:
		break;

	default:
		break;
	}
}

LRESULT Gear::WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
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

	case WM_MOUSEMOVE:
	{
		const float curX = (float)GET_X_LPARAM(lParam);
		const float curY = (float)Graphics::getHeight() - (float)GET_Y_LPARAM(lParam);

		Mouse::dx = curX - Mouse::x;
		Mouse::dy = curY - Mouse::y;
		Mouse::x = curX;
		Mouse::y = curY;

		Mouse::moveEvent();

	}
	break;

	case WM_LBUTTONDOWN:
		Mouse::leftDown = true;
		Mouse::leftDownEvent();
		break;

	case WM_RBUTTONDOWN:
		Mouse::rightDown = true;
		Mouse::rightDownEvent();
		break;

	case WM_LBUTTONUP:
		Mouse::leftDown = false;
		Mouse::leftUpEvent();
		break;

	case WM_RBUTTONUP:
		Mouse::rightDown = false;
		Mouse::rightUpEvent();
		break;

	case WM_MOUSEWHEEL:
		Mouse::wheelDelta = (float)GET_WHEEL_DELTA_WPARAM(wParam) / 120.f;
		Mouse::scrollEvent();
		break;

	case WM_KEYDOWN:
		if ((HIWORD(lParam) & KF_REPEAT) == 0)
		{
			Keyboard::keyDownMap[(Keyboard::Key)wParam] = true;
			Keyboard::keyDownEvents[(Keyboard::Key)wParam]();
		}
		break;

	case WM_KEYUP:
		Keyboard::keyDownMap[(Keyboard::Key)wParam] = false;
		Keyboard::keyUpEvents[(Keyboard::Key)wParam]();
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}

	return 0;
}