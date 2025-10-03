#include<Gear/Gear.h>

#include<Gear/Core/Graphics.h>

#include<Gear/Core/VideoEncoder/NvidiaEncoder.h>

#include<Gear/Utils/Logger.h>

#include<dxgidebug.h>

Gear* Gear::instance = nullptr;

Gear* Gear::get()
{
	return instance;
}

int Gear::iniEngine(const Configuration& config, const int argc, const wchar_t* argv[])
{
	Logger::instance = new Logger();

	Utils::exeRootPath = Utils::File::backslashToSlash(Utils::File::getParentFolder(argv[0]));

	LOGENGINE(L"executable path", LogColor::brightBlue, Utils::getRootFolder());

	usage = config.usage;

	iniWindow(config.title, config.width, config.height);

	if (config.usage == Configuration::EngineUsage::VIDEOPLAYBACK)
	{
		ShowWindow(winform->getHandle(), SW_HIDE);
	}

	switch (config.usage)
	{
	case Configuration::EngineUsage::NORMAL:

		RenderEngine::instance = new RenderEngine(config.width, config.height, winform->getHandle(), true, config.enableImGuiSurface);

		LOGENGINE(L"engine usage normal");

		break;

	case Configuration::EngineUsage::VIDEOPLAYBACK:

		RenderEngine::instance = new RenderEngine(config.width, config.height, winform->getHandle(), false, false);

		LOGENGINE(L"engine usage video playback");

		break;

	default:
		break;
	}

	LOGENGINE(L"resolution", Graphics::getWidth(), L"x", Graphics::getHeight());

	LOGENGINE(L"aspect ratio", Graphics::getAspectRatio());

	LOGENGINE(L"back buffer count", Graphics::getFrameBufferCount());

	return 0;
}

void Gear::iniGame(Game* const gamePtr)
{
	game = gamePtr;

	RenderEngine::get()->initializeResources();

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
	RenderEngine::get()->setDeltaTime(0.0001f);

	while (winform->pollEvents())
	{
		const std::chrono::high_resolution_clock::time_point startPoint = std::chrono::high_resolution_clock::now();

		RenderEngine::get()->setDefRenderTexture();

		RenderEngine::get()->begin();

		game->update(Graphics::getDeltaTime());

		game->render();

		RenderEngine::get()->end();

		RenderEngine::get()->present();

		RenderEngine::get()->waitForNextFrame();

		Mouse::resetDeltaInfo();

		Keyboard::resetOnKeyDownMap();

		const std::chrono::high_resolution_clock::time_point endPoint = std::chrono::high_resolution_clock::now();

		const float deltaTime = std::chrono::duration<float>(endPoint - startPoint).count();

		const float lerpDeltaTime = dtEstimator.getDeltaTime(deltaTime);

		RenderEngine::get()->setDeltaTime(lerpDeltaTime);

		RenderEngine::get()->updateTimeElapsed();
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

	if (vendor == GPUVendor::NVIDIA)
	{
		const uint32_t numTextures = NvidiaEncoder::lookaheadDepth + 1;

		Texture* renderTextures[numTextures] = {};

		D3D12_CPU_DESCRIPTOR_HANDLE textureHandles[numTextures] = {};

		{
			DescriptorHandle descriptorHandle = GlobalDescriptorHeap::getRenderTargetHeap()->allocStaticDescriptor(numTextures);

			D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};
			rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
			rtvDesc.Format = Graphics::backBufferFormat;
			rtvDesc.Texture2D.MipSlice = 0;
			rtvDesc.Texture2D.PlaneSlice = 0;

			for (uint32_t i = 0; i < numTextures; i++)
			{
				const D3D12_CLEAR_VALUE clearValue = { Graphics::backBufferFormat ,{0.f,0.f,0.f,1.f} };

				renderTextures[i] = new Texture(Graphics::getWidth(), Graphics::getHeight(), Graphics::backBufferFormat, 1, 1, true, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET, &clearValue);

				GraphicsDevice::get()->CreateRenderTargetView(renderTextures[i]->getResource(), &rtvDesc, descriptorHandle.getCPUHandle());

				textureHandles[i] = descriptorHandle.getCPUHandle();

				descriptorHandle.move();
			}
		}

		uint32_t index = 0;

		RenderEngine::get()->setDeltaTime(1.f / static_cast<float>(Encoder::frameRate));

		while (true)
		{
			RenderEngine::get()->setRenderTexture(renderTextures[index], textureHandles[index]);

			RenderEngine::get()->begin();

			game->update(Graphics::getDeltaTime());

			game->render();

			RenderEngine::get()->end();

			RenderEngine::get()->waitForCurrentFrame();

			RenderEngine::get()->updateTimeElapsed();

			if (!encoder->encode(RenderEngine::get()->getRenderTexture()))
			{
				break;
			}

			index = (index + 1) % numTextures;
		}

		for (uint32_t i = 0; i < numTextures; i++)
		{
			delete renderTextures[i];
		}
	}

	if (encoder)
	{
		delete encoder;
	}

	std::cin.get();
}

void Gear::reportLiveObjects() const
{
	ComPtr<IDXGIDebug1> dxgiDebug;

	OutputDebugStringA("**********Live Object Report**********\n");

	if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&dxgiDebug))))
	{
		dxgiDebug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_FLAGS(DXGI_DEBUG_RLO_ALL | DXGI_DEBUG_RLO_IGNORE_INTERNAL));
	}

	OutputDebugStringA("**********Live Object Report**********\n");
}

Gear::Gear() :
	game(nullptr), winform(nullptr), usage(Configuration::EngineUsage::NORMAL)
{

}

Gear::~Gear()
{
	if (RenderEngine::instance)
	{
		RenderEngine::get()->waitForCurrentFrame();
	}

	if (game)
	{
		delete game;
	}

	if (winform)
	{
		delete winform;
	}

	if (RenderEngine::instance)
	{
		delete RenderEngine::instance;
	}

	if (Logger::instance)
	{
		delete Logger::instance;
	}

#ifdef _DEBUG

	reportLiveObjects();

#endif // _DEBUG
}

void Gear::iniWindow(const std::wstring& title, const uint32_t width, const uint32_t height)
{
	switch (usage)
	{
	case Configuration::EngineUsage::NORMAL:
		winform = new Win32Form(title, width, height, normalWndStyle, Gear::windowCallback);
		break;

	case Configuration::EngineUsage::VIDEOPLAYBACK:
		winform = new Win32Form(title, 100, 100, normalWndStyle, Gear::encodeCallback);
		break;

	default:
		break;
	}
}

LRESULT Gear::windowCallback(HWND hWnd, uint32_t uMsg, WPARAM wParam, LPARAM lParam)
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

LRESULT Gear::encodeCallback(HWND hWnd, uint32_t uMsg, WPARAM wParam, LPARAM lParam)
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
