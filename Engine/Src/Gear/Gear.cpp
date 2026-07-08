#include<Gear/Gear.h>

#include<Gear/Window/Win32Form.h>

#include<Gear/Core/RenderEngine.h>

#include<Gear/Core/Internal/RenderEngineInternal.h>

#include<Gear/Core/Graphics.h>

#include<Gear/Input/Keyboard.h>

#include<Gear/Utils/MainMonitor.h>

#include<Gear/Utils/Internal/MainMonitorInternal.h>

#include<Gear/Utils/File.h>

#include<Gear/Utils/Internal/FileInternal.h>

#include<Gear/Utils/Logger.h>

#include<Gear/Utils/Internal/LoggerInternal.h>

#include<Gear/Utils/WallpaperHelper.h>

#include<Gear/Utils/DeltaTimeEstimator.h>

#include<Gear/Core/VideoEncoder/NVIDIAEncoder.h>

#include<iostream>

#include<locale>

#include<dxgidebug.h>

#include<dxgi1_6.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION

#include<stb_image_write.h>

namespace Gear
{
	class GearImpl
	{
	public:

		GearImpl(const GearImpl&) = delete;

		void operator=(const GearImpl&) = delete;

		GearImpl(const InitializationParam& param, const int32_t argc, const wchar_t* argv[]);

		~GearImpl();

		void initGame(UniquePtr<Game> gamePtr);

	private:

		void runRealTimeRender();

		void runVideoRender();

		void runWallpaper();

		void reportLiveObjects() const;

		UniquePtr<Logger::Internal::InitializeToken> loggerToken;

		UniquePtr<File::Internal::InitializeToken> fileToken;

		UniquePtr<Window::Win32Form::InitializeToken> windowToken;

		UniquePtr<RenderEngine::Internal::InitializeToken> renderEngineToken;

		UniquePtr<Game> game;

		//用于截屏
		UniquePtr<D3D12Resource::ReadbackHeap> backBufferHeap;

		InitializationParam::EngineUsage usage;

		union
		{
			InitializationParam::RealTimeRenderParam realTimeRender;

			InitializationParam::VideoRenderParam videoRender;
		};

		static constexpr Input::Keyboard::Key screenGrabKey = Input::Keyboard::F11;

	};

	GearImpl::GearImpl(const InitializationParam& param, const int32_t argc, const wchar_t* argv[])
	{
		//设置locale为.UTF-8用于多语言支持
		std::locale::global(std::locale(".UTF-8"));

		loggerToken = makeUnique<Logger::Internal::InitializeToken>();

		fileToken = makeUnique<File::Internal::InitializeToken>(File::backslashToSlash(File::getParentFolder(argv[0])));

		LOGENGINE("EXE根目录", LogColor::brightBlue, File::getWRootFolder());

		MainMonitor::Internal::getCurrentSettings();

		usage = param.usage;

		switch (usage)
		{
		case InitializationParam::EngineUsage::REALTIMERENDER:

			realTimeRender = param.realTimeRender;

			windowToken = makeUnique<Window::Win32Form::InitializeToken>(param.title,
				(MainMonitor::getWidth() - realTimeRender.width) / 2, (MainMonitor::getHeight() - realTimeRender.height) / 2,
				realTimeRender.width, realTimeRender.height, Window::Win32Form::normalWindowStyle, Window::Win32Form::windowCallback);

			renderEngineToken = makeUnique<RenderEngine::Internal::InitializeToken>(realTimeRender.width, realTimeRender.height, Window::Win32Form::getHandle(), true, realTimeRender.enableImGuiSurface);

			backBufferHeap = makeUnique<D3D12Resource::ReadbackHeap>(FMT::getByteSize(Graphics::backBufferFormat) * realTimeRender.width * realTimeRender.height);

			SetForegroundWindow(Window::Win32Form::getHandle());

			LOGENGINE("引擎用途", "实时渲染");

			break;

		case InitializationParam::EngineUsage::VIDEORENDER:

			videoRender = param.videoRender;

			windowToken = makeUnique<Window::Win32Form::InitializeToken>(param.title, 100, 100, 100, 100, Window::Win32Form::normalWindowStyle, Window::Win32Form::encodeCallback);

			ShowWindow(Window::Win32Form::getHandle(), SW_HIDE);

			renderEngineToken = makeUnique<RenderEngine::Internal::InitializeToken>(videoRender.width, videoRender.height, Window::Win32Form::getHandle(), false, false);

			LOGENGINE("引擎用途", "视频渲染");

			break;

		case InitializationParam::EngineUsage::WALLPAPER:

			windowToken = makeUnique<Window::Win32Form::InitializeToken>(param.title, 0, 0, MainMonitor::getWidth(), MainMonitor::getHeight(), Window::Win32Form::wallpaperWindowStyle, Window::Win32Form::wallpaperCallBack);

			{
				const HWND parentHWND = WallpaperHelper::getWallpaperHWND();

				SetParent(Window::Win32Form::getHandle(), parentHWND);
			}

			renderEngineToken = makeUnique<RenderEngine::Internal::InitializeToken>(MainMonitor::getWidth(), MainMonitor::getHeight(), Window::Win32Form::getHandle(), true, false);

			LOGENGINE("引擎用途", "动态壁纸");

			break;

		default:
			break;
		}

		LOGENGINE("分辨率", Graphics::getWidth(), "x", Graphics::getHeight());

		LOGENGINE("横纵比", Graphics::getAspectRatio());

		LOGENGINE("后备缓冲数量", Graphics::getFrameBufferCount());
	}

	GearImpl::~GearImpl()
	{
		RenderEngine::Internal::waitDestroyable();

#ifdef _DEBUG

		//由于Debug模式下需要输出存活的对象，所以需要手动释放相关资源
		backBufferHeap.reset();

		game.reset();

		renderEngineToken.reset();

		reportLiveObjects();

#endif // _DEBUG

		LOGENGINE("资源销毁完毕");
	}

	void GearImpl::initGame(UniquePtr<Game> gamePtr)
	{
		game = std::move(gamePtr);

		RenderEngine::Internal::initializeResources();

		switch (usage)
		{
		case InitializationParam::EngineUsage::REALTIMERENDER:
			return runRealTimeRender();
		case InitializationParam::EngineUsage::VIDEORENDER:
			return runVideoRender();
		case InitializationParam::EngineUsage::WALLPAPER:
			return runWallpaper();
		default:
			return;
		}
	}

	void GearImpl::runRealTimeRender()
	{
		DeltaTimeEstimator dtEstimator;

		RenderEngine::Internal::setDeltaTime(1.f / static_cast<float>(MainMonitor::getRefreshRate()));

		while (Window::Win32Form::pollEvents())
		{
			const std::chrono::high_resolution_clock::time_point startPoint = std::chrono::high_resolution_clock::now();

			//帧索引的更新必须置于首位
			RenderEngine::Internal::updateFrameIndex();

			RenderEngine::Internal::waitFrameCPUReusable();

			RenderEngine::Internal::setDefRenderTexture();

			RenderEngine::Internal::beginFrame();

			game->update(Graphics::getDeltaTime());

			game->render();

			const bool needScreenGrab = Input::Keyboard::onKeyDown(screenGrabKey);

			if (needScreenGrab)
			{
				RenderEngine::Internal::saveBackBuffer(backBufferHeap.get());
			}

			RenderEngine::Internal::endFrame();

			RenderEngine::Internal::present();

			if (needScreenGrab)
			{
				RenderEngine::Internal::waitFrameGPUComplete();
			}

			const std::chrono::high_resolution_clock::time_point endPoint = std::chrono::high_resolution_clock::now();

			const float deltaTime = std::chrono::duration<float>(endPoint - startPoint).count();

			const float lerpDeltaTime = dtEstimator.getDeltaTime(deltaTime);

			RenderEngine::Internal::setDeltaTime(lerpDeltaTime);

			RenderEngine::Internal::updateTimeElapsed();

			if (needScreenGrab)
			{
				const uint8_t* const dataPtr = reinterpret_cast<uint8_t*>(backBufferHeap->map(CD3DX12_RANGE(0ull,
					FMT::getByteSize(Graphics::backBufferFormat) * realTimeRender.width * realTimeRender.height)));

				UniquePtr<uint8_t[]> colors = makeUnique<uint8_t[]>(FMT::getByteSize(Graphics::backBufferFormat) * realTimeRender.width * realTimeRender.height);

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

				stbi_write_png("output.png", realTimeRender.width, realTimeRender.height, 4, colors.get(), FMT::getByteSize(Graphics::backBufferFormat) * realTimeRender.width);

				LOGSUCCESS("截屏保存到", "output.png");
			}
		}
	}

	void GearImpl::runVideoRender()
	{
		const AdapterVendor vendor = RenderEngine::getVendor();

		UniquePtr<VideoEncoder::Encoder> encoder;

		const uint32_t frameToEncode = videoRender.second * VideoEncoder::Encoder::frameRate;

		switch (vendor)
		{
		case AdapterVendor::NVIDIA:
			encoder = makeUnique<VideoEncoder::NVIDIAEncoder>(frameToEncode, videoRender.bFrames);
			break;
		case AdapterVendor::AMD:
		case AdapterVendor::INTEL:
		case AdapterVendor::UNKNOWN:
			LOGERROR("目前只为英伟达的GPU做了视频编码的适配");
			break;
		default:
			break;
		}

		UniquePtr<D3D12Resource::Texture> renderTexture = makeUnique<D3D12Resource::Texture>(Graphics::getWidth(), Graphics::getHeight(), Graphics::backBufferFormat, 1, 1, true, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET, nullptr);

		D3D12_CPU_DESCRIPTOR_HANDLE textureHandle;

		{
			DescriptorHandle descriptorHandle = LocalDescriptorHeap::getRenderTargetHeap()->allocStaticDescriptor(1);

			D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};
			rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
			rtvDesc.Format = Graphics::backBufferFormat;
			rtvDesc.Texture2D.MipSlice = 0;
			rtvDesc.Texture2D.PlaneSlice = 0;

			GraphicsDevice::get()->CreateRenderTargetView(renderTexture->getResource(), &rtvDesc, descriptorHandle.getCurrentCPUHandle());

			textureHandle = descriptorHandle.getCurrentCPUHandle();
		}

		RenderEngine::Internal::setDeltaTime(1.f / static_cast<float>(VideoEncoder::Encoder::frameRate));

		D3D12Core::FencePtr vpSyncFence = makeUnique<D3D12Core::Fence>();

		do
		{

			RenderEngine::Internal::setRenderTexture(renderTexture.get(), textureHandle);

			RenderEngine::Internal::beginFrame();

			game->update(Graphics::getDeltaTime());

			game->render();

			RenderEngine::Internal::endFrame();

			RenderEngine::Internal::updateTimeElapsed();

			RenderEngine::Internal::waitFrameGPUComplete();

			//编码器管理的命令队列需要等待渲染引擎管理的命令队列完成工作
			encoder->waitFor(RenderEngine::getCommandQueue(), vpSyncFence.get());

		} while (encoder->encode(RenderEngine::getRenderTexture()));
	}

	void GearImpl::runWallpaper()
	{
		DeltaTimeEstimator dtEstimator;

		WallpaperHelper::DetectThreadToken detectThreadToken;

		RenderEngine::Internal::setDeltaTime(1.f / static_cast<float>(MainMonitor::getRefreshRate()));

		while (Window::Win32Form::pollEvents())
		{
			while (WallpaperHelper::isDesktopObscured())
			{
				if (!Window::Win32Form::pollEvents(static_cast<DWORD>(WallpaperHelper::obscureCheckInterval / 2ull - 75ull)))
				{
					return;
				}
			}

			const std::chrono::high_resolution_clock::time_point startPoint = std::chrono::high_resolution_clock::now();

			//帧索引的更新必须置于首位
			RenderEngine::Internal::updateFrameIndex();

			RenderEngine::Internal::waitFrameCPUReusable();

			RenderEngine::Internal::setDefRenderTexture();

			RenderEngine::Internal::beginFrame();

			game->update(Graphics::getDeltaTime());

			game->render();

			RenderEngine::Internal::endFrame();

			RenderEngine::Internal::present();

			const std::chrono::high_resolution_clock::time_point endPoint = std::chrono::high_resolution_clock::now();

			const float deltaTime = std::chrono::duration<float>(endPoint - startPoint).count();

			const float lerpDeltaTime = dtEstimator.getDeltaTime(deltaTime);

			RenderEngine::Internal::setDeltaTime(lerpDeltaTime);

			RenderEngine::Internal::updateTimeElapsed();
		}
	}

	void GearImpl::reportLiveObjects() const
	{
		ComPtr<IDXGIDebug1> dxgiDebug;

		OutputDebugStringA("**********Live Object Report**********\n");

		if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&dxgiDebug))))
		{
			dxgiDebug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_FLAGS(DXGI_DEBUG_RLO_ALL | DXGI_DEBUG_RLO_IGNORE_INTERNAL));
		}

		OutputDebugStringA("**********Live Object Report**********\n");
	}

	UniquePtr<GearImpl> impl;

	void initialize(const InitializationParam& param, const int32_t argc, const wchar_t* argv[])
	{
		impl = makeUnique<GearImpl>(param, argc, argv);
	}

	void release()
	{
		impl.reset();
	}

	void initGame(UniquePtr<Game> gamePtr)
	{
		impl->initGame(std::move(gamePtr));
	}

	void failureExit(const std::exception& e)
	{
		//保证错误信息被输出到log.txt
		Logger::Internal::release();

		std::cerr << e.what();

		std::cin.get();

		std::quick_exit(EXIT_FAILURE);
	}
}
