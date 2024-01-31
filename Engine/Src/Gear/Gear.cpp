#include<Gear/Gear.h>

Gear Gear::instance;

Gear& Gear::get()
{
	return instance;
}

int Gear::iniEngine(const Configuration& config, const int& argc, const char* argv[])
{
	std::wcout.imbue(std::locale(""));

	{
		std::string exeRootPath = argv[0];

		std::cout << typeid(*this).name() << " executable path " << exeRootPath << "\n";
	}

	usage = config.usage;

	iniWindow(config.title, config.width, config.height);

	return 0;
}

void Gear::iniGame(Game* const game)
{
	this->game = game;

	switch (usage)
	{
	default:
	case Configuration::EngineUsage::WALLPAPER:
	case Configuration::EngineUsage::NORMAL:
		runGame();
		break;

	case Configuration::EngineUsage::VIDEOPLAYBACK:
		break;
	}

	destroy();
}

void Gear::runGame()
{
	std::chrono::high_resolution_clock clock;

	while (winform->pollEvents())
	{
		const std::chrono::high_resolution_clock::time_point startPoint = clock.now();

		game->update(0.f);

		game->render();

		RenderEngine::get()->processCommandLists();

		RenderEngine::get()->present();

		const std::chrono::high_resolution_clock::time_point endPoint = clock.now();

		Graphics::time.deltaTime = std::chrono::duration_cast<std::chrono::milliseconds>(endPoint - startPoint).count() / 1000.f;
		Graphics::time.timeElapsed += Graphics::time.deltaTime;
	}

}

void Gear::destroy()
{
	delete game;

	delete winform;
}

Gear::Gear()
{

}

void Gear::iniWindow(const std::wstring& title, const UINT& width, const UINT& height)
{
	switch (usage)
	{
	case Configuration::EngineUsage::NORMAL:
		std::cout << typeid(*this).name() << " usage normal\n";
		winform = new Win32Form(title, width, height, normalWndStyle, Gear::WindowProc);
		break;

	case Configuration::EngineUsage::WALLPAPER:
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

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}

	return 0;
}