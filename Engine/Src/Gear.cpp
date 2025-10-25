#include<Gear/Gear.h>

#include<Gear/Core/RenderEngine.h>

#include<Gear/Window/Win32Form.h>

#include<Gear/Utils/WallpaperHelper.h>

#include<Gear/Core/Graphics.h>

#include<Gear/Core/VideoEncoder/NvidiaEncoder.h>

#include<Gear/Utils/Logger.h>

#include<Gear/Utils/Internal/LoggerInternal.h>

#include<Gear/Utils/Internal/FileInternal.h>

#include<dxgidebug.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION

#include<stb_image_write.h>

Gear* Gear::instance = nullptr;

Gear* Gear::get()
{
	return instance;
}

int32_t Gear::iniEngine(const InitializationParam& param, const int32_t argc, const wchar_t* argv[])
{
	Utils::Logger::Internal::initialize();

	Utils::File::Internal::setRootFolder(Utils::File::backslashToSlash(Utils::File::getParentFolder(argv[0])));

	LOGENGINE(L"root folder", LogColor::brightBlue, Utils::File::getRootFolder());

	usage = param.usage;

	DirectX::XMUINT2 systemResolution = {};

	Utils::WallpaperHelper::getSystemResolution(systemResolution.x, systemResolution.y);

	switch (usage)
	{
	case InitializationParam::EngineUsage::REALTIMERENDER:

		realTimeRender = param.realTimeRender;

		Win32Form::instance = new Win32Form(param.title, (systemResolution.x - realTimeRender.width) / 2, (systemResolution.y - realTimeRender.height) / 2,
			realTimeRender.width, realTimeRender.height, Win32Form::normalWindowStyle, Win32Form::windowCallback);

		RenderEngine::instance = new RenderEngine(realTimeRender.width, realTimeRender.height, Win32Form::get()->getHandle(), true, realTimeRender.enableImGuiSurface);

		backBufferHeap = new ReadbackHeap(FMT::getByteSize(Graphics::backBufferFormat) * realTimeRender.width * realTimeRender.height);

		LOGENGINE(L"engine usage real time render");

		break;

	case InitializationParam::EngineUsage::VIDEORENDER:

		videoRender = param.videoRender;

		Win32Form::instance = new Win32Form(param.title, 100, 100, 100, 100, Win32Form::normalWindowStyle, Win32Form::encodeCallback);

		ShowWindow(Win32Form::get()->getHandle(), SW_HIDE);

		RenderEngine::instance = new RenderEngine(videoRender.width, videoRender.height, Win32Form::get()->getHandle(), false, false);

		LOGENGINE(L"engine usage video render");

		break;

	case InitializationParam::EngineUsage::WALLPAPER:

		Win32Form::instance = new Win32Form(param.title, 0, 0, systemResolution.x, systemResolution.y, Win32Form::wallpaperWindowStyle, Win32Form::wallpaperCallBack);

		{
			const HWND parentHWND = Utils::WallpaperHelper::getWallpaperHWND();

			SetParent(Win32Form::get()->getHandle(), parentHWND);
		}

		RenderEngine::instance = new RenderEngine(systemResolution.x, systemResolution.y, Win32Form::get()->getHandle(), true, false);

		LOGENGINE(L"engine usage wallpaper");

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
	case InitializationParam::EngineUsage::REALTIMERENDER:
		runRealTimeRender();
		break;

	case InitializationParam::EngineUsage::VIDEORENDER:
		runVideoRender();
		break;

	case InitializationParam::EngineUsage::WALLPAPER:
		runWallpaper();
		break;

	default:
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

void Gear::runRealTimeRender()
{
	Utils::DeltaTimeEstimator dtEstimator;

	while (Win32Form::get()->pollEvents())
	{
		const std::chrono::high_resolution_clock::time_point startPoint = std::chrono::high_resolution_clock::now();

		RenderEngine::get()->setDefRenderTexture();

		RenderEngine::get()->begin();

		game->update(Graphics::getDeltaTime());

		game->render();

		const bool needScreenGrab = Input::Keyboard::onKeyDown(screenGrabKey);

		if (needScreenGrab)
		{
			RenderEngine::get()->saveBackBuffer(backBufferHeap);
		}

		RenderEngine::get()->end();

		RenderEngine::get()->present();

		if (needScreenGrab)
		{
			RenderEngine::get()->waitForCurrentFrame();
		}

		RenderEngine::get()->waitForNextFrame();

		const std::chrono::high_resolution_clock::time_point endPoint = std::chrono::high_resolution_clock::now();

		const float deltaTime = std::chrono::duration<float>(endPoint - startPoint).count();

		const float lerpDeltaTime = dtEstimator.getDeltaTime(deltaTime);

		RenderEngine::get()->setDeltaTime(lerpDeltaTime);

		RenderEngine::get()->updateTimeElapsed();

		if (needScreenGrab)
		{
			const uint8_t* const dataPtr = reinterpret_cast<uint8_t*>(backBufferHeap->map(CD3DX12_RANGE(0ull,
				FMT::getByteSize(Graphics::backBufferFormat) * realTimeRender.width * realTimeRender.height)));

			uint8_t* const colors = new uint8_t[FMT::getByteSize(Graphics::backBufferFormat) * realTimeRender.width * realTimeRender.height];

			for (uint32_t i = 0; i < realTimeRender.width * realTimeRender.height; i++)
			{
				const uint32_t pixel = 4 * i;

				//RGBA <- BGRA
				colors[pixel] = dataPtr[pixel + 2];

				colors[pixel + 1] = dataPtr[pixel + 1];

				colors[pixel + 2] = dataPtr[pixel];

				colors[pixel + 3] = 0xFFu;
			}

			backBufferHeap->unmap();

			stbi_write_png("output.png", realTimeRender.width, realTimeRender.height, 4, colors, FMT::getByteSize(Graphics::backBufferFormat) * realTimeRender.width);

			delete[] colors;

			LOGSUCCESS(L"screenshot saved to output.png");
		}
	}
}

void Gear::runVideoRender()
{
	const GPUVendor vendor = RenderEngine::get()->getVendor();

	Encoder* encoder = nullptr;

	const uint32_t frameToEncode = videoRender.second * Encoder::frameRate;

	switch (vendor)
	{
	case GPUVendor::NVIDIA:
		encoder = new NvidiaEncoder(frameToEncode);
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
}

void Gear::runWallpaper()
{
	Utils::DeltaTimeEstimator dtEstimator;

	while (Win32Form::get()->pollEvents())
	{
		const std::chrono::high_resolution_clock::time_point startPoint = std::chrono::high_resolution_clock::now();

		RenderEngine::get()->setDefRenderTexture();

		RenderEngine::get()->begin();

		game->update(Graphics::getDeltaTime());

		game->render();

		RenderEngine::get()->end();

		RenderEngine::get()->present();

		RenderEngine::get()->waitForNextFrame();

		const std::chrono::high_resolution_clock::time_point endPoint = std::chrono::high_resolution_clock::now();

		const float deltaTime = std::chrono::duration<float>(endPoint - startPoint).count();

		const float lerpDeltaTime = dtEstimator.getDeltaTime(deltaTime);

		RenderEngine::get()->setDeltaTime(lerpDeltaTime);

		RenderEngine::get()->updateTimeElapsed();
	}
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
	game(nullptr), backBufferHeap(nullptr)
{

}

Gear::~Gear()
{
	LOGENGINE(L"destroy resources");

	if (RenderEngine::instance)
	{
		RenderEngine::get()->waitForCurrentFrame();
	}

	if (backBufferHeap)
	{
		delete backBufferHeap;
	}

	if (game)
	{
		delete game;
	}

	if (Win32Form::instance)
	{
		delete Win32Form::instance;
	}

	if (RenderEngine::instance)
	{
		delete RenderEngine::instance;
	}

	LOGSUCCESS(L"engine exit");

	Utils::Logger::Internal::release();

#ifdef _DEBUG

	reportLiveObjects();

#endif // _DEBUG

}
