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

		Utils::exeRootPath = Utils::File::backslashToSlash(Utils::File::getParentFolder(exeRootPath));

		std::cout << "[class Gear] executable path " << Utils::exeRootPath << "\n";
	}

	usage = config.usage;

	iniWindow(config.title, config.width, config.height);

	if (config.usage == Configuration::EngineUsage::VIDEOPLAYBACK)
	{
		ShowWindow(winform->getHandle(), SW_HIDE);
	}

	Graphics::width = config.width;
	Graphics::height = config.height;
	Graphics::aspectRatio = static_cast<float>(Graphics::width) / static_cast<float>(Graphics::height);

	switch (config.usage)
	{
	case Configuration::EngineUsage::NORMAL:
		RenderEngine::instance = new RenderEngine(winform->getHandle(), true, config.enableImGuiSurface);
		break;

	case Configuration::EngineUsage::VIDEOPLAYBACK:
		RenderEngine::instance = new RenderEngine(winform->getHandle(), false, false);
		break;

	default:
		break;
	}

	std::cout << "[class Gear] resolution " << Graphics::width << "x" << Graphics::height << "\n";
	std::cout << "[class Gear] aspect ratio " << Graphics::aspectRatio << "\n";
	std::cout << "[class Gear] back buffer count " << Graphics::FrameBufferCount << "\n";

	switch (usage)
	{
	default:
	case Configuration::EngineUsage::NORMAL:
		std::cout << "[class Gear] usage normal\n";
		break;
	case Configuration::EngineUsage::VIDEOPLAYBACK:
		std::cout << "[class Gear] usage video playback\n";
		break;
	}

	return 0;
}

void Gear::iniGame(Game* const gamePtr)
{
	game = gamePtr;

	//resource creation may need dynamic constant buffer
	RenderEngine::get()->updateConstantBuffer();

	RenderEngine::get()->processCommandLists();

	RenderEngine::get()->waitForPreviousFrame();

	switch (usage)
	{
	default:
	case Configuration::EngineUsage::NORMAL:
		runGame();
		break;
	case Configuration::EngineUsage::VIDEOPLAYBACK:
		runEncode();
		break;
	}

	RenderEngine::get()->waitForPreviousFrame();

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
	std::chrono::high_resolution_clock clock = std::chrono::high_resolution_clock();

	while (winform->pollEvents())
	{
		const std::chrono::high_resolution_clock::time_point startPoint = clock.now();

		RenderEngine::get()->begin();

		game->update(Graphics::time.deltaTime);

		game->render();

		RenderEngine::get()->end();

		RenderEngine::get()->waitForNextFrame();

		Mouse::resetDeltaInfo();

		Keyboard::resetOnKeyDownMap();

		const std::chrono::high_resolution_clock::time_point endPoint = clock.now();

		Graphics::time.deltaTime = std::chrono::duration_cast<std::chrono::milliseconds>(endPoint - startPoint).count() / 1000.f;

		Graphics::time.timeElapsed += Graphics::time.deltaTime;

		Graphics::time.uintSeed = Random::Uint();

		Graphics::time.floatSeed = Random::Float();
	}

}

void Gear::runEncode()
{
	GPUVendor vendor = RenderEngine::get()->getVendor();

	Encoder* encoder = nullptr;

	switch (vendor)
	{
	case GPUVendor::NVIDIA:
		encoder = new NvidiaEncoder(3600);
		break;
	case GPUVendor::AMD:
	case GPUVendor::INTEL:
	case GPUVendor::UNKNOWN:
		break;
	default:
		break;
	}

	Graphics::time.deltaTime = 1.f / 60.f;

	do
	{
		RenderEngine::get()->begin();

		game->update(Graphics::time.deltaTime);

		game->render();

		RenderEngine::get()->end();

		RenderEngine::get()->waitForPreviousFrame();

		Graphics::time.timeElapsed += Graphics::time.deltaTime;

		Graphics::time.uintSeed = Random::Uint();

		Graphics::time.floatSeed = Random::Float();

	} while (encoder->encode(RenderEngine::get()->getCurrentRenderTexture()));

	delete encoder;

	std::cin.get();
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

#ifdef _DEBUG
	if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&dxgiDebug))))
	{
		dxgiDebug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_FLAGS(DXGI_DEBUG_RLO_DETAIL | DXGI_DEBUG_RLO_IGNORE_INTERNAL));
	}
#endif
}

Gear::Gear() :
	game(nullptr), winform(nullptr), usage(Configuration::EngineUsage::NORMAL)
{

}

Gear::~Gear()
{

}

void Gear::iniWindow(const std::wstring title, const UINT width, const UINT height)
{
	switch (usage)
	{
	case Configuration::EngineUsage::NORMAL:
		winform = new Win32Form(title, width, height, normalWndStyle, Gear::WindowProc);
		break;

	case Configuration::EngineUsage::VIDEOPLAYBACK:
		winform = new Win32Form(title, 100, 100, normalWndStyle, Gear::EncodeProc);
		break;

	default:
		break;
	}
}

LRESULT Gear::WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
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
	{
		const float curX = (float)GET_X_LPARAM(lParam);
		const float curY = (float)Graphics::getHeight() - (float)GET_Y_LPARAM(lParam);

		Mouse::dx = curX - Mouse::x;
		Mouse::dy = curY - Mouse::y;
		Mouse::x = curX;
		Mouse::y = curY;

		Mouse::onMoved = true;

		Mouse::moveEvent();
	}
	break;

	case WM_LBUTTONDOWN:
		Mouse::leftDown = true;
		Mouse::onLeftDowned = true;
		Mouse::leftDownEvent();
		break;

	case WM_RBUTTONDOWN:
		Mouse::rightDown = true;
		Mouse::onRightDowned = true;
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
		Mouse::onScrolled = true;
		Mouse::scrollEvent();
		break;

	case WM_KEYDOWN:
		if ((HIWORD(lParam) & KF_REPEAT) == 0)
		{
			Keyboard::keyDownMap[(Keyboard::Key)wParam] = true;
			Keyboard::onKeyDownMap[(Keyboard::Key)wParam] = true;
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

LRESULT Gear::EncodeProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
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
