#include "Engine.h"
#include "Application.h"
#include "Platform/Platform.h"
#include "NotificationManager.h"
#include "Window.h"

Core::Engine::Engine()
{
	NotificationManagerInstance = std::make_unique<NotificationManager>();

	if (!Platform::Initialize(NotificationManagerInstance.get()))
	{
		return;
	}
}

Core::Engine::~Engine()
{
}

bool Core::Engine::BeginApplication(std::unique_ptr<Application> pApplication)
{
	if ((RunningApplicationInstance) || (!pApplication))
	{
		return false;
	}

	ApplicationInstance = std::move(pApplication);
	RunningApplicationInstance = true;

	BeginApplicationMainLoop();

	return SignalRestart;
}

bool Core::Engine::CreateConsoleWindow()
{
	return Platform::CreateConsole();
}

bool Core::Engine::RemoveConsoleWindow()
{
	return Platform::RemoveConsole();
}

bool Core::Engine::DestroyWindowOnPlatform(Core::Window& WindowToDestroy)
{
	if (Platform::DestroyPlatformWindow(WindowToDestroy))
	{
		WindowToDestroy.SetPlatformHandle(nullptr);
		return true;
	}
	return false;
}

bool Core::Engine::MakeWindowFullscreenOnPlatform(Core::Window& WindowToMakeFullscreen)
{
	return Platform::MakePlatformWindowFullscreen(WindowToMakeFullscreen);
}

bool Core::Engine::ExitWindowFullscreenOnPlatform(Core::Window& WindowToExitFullscreen)
{
	return Platform::ExitPlatformWindowFullscreen(WindowToExitFullscreen);
}

bool Core::Engine::CaptureCursorOnPlatform(Core::Rectangle& CaptureRegion)
{
	return Platform::CaptureCursor(CaptureRegion);
}

bool Core::Engine::UncaptureCursorOnPlatform()
{
	return Platform::UncaptureCursor();
}

Core::Rectangle Core::Engine::GetWindowRegionOnPlatform(const Core::Window& TargetWindow)
{
	return Platform::GetPlatformWindowRegion(TargetWindow);
}

Core::Rectangle Core::Engine::GetWindowClientRegionOnPlatform(const Core::Window& TargetWindow)
{
	return Platform::GetPlatformWindowClientRegion(TargetWindow);
}

Core::NotificationManager& Core::Engine::GetNotificationManager()
{
	return *NotificationManagerInstance;
}

void Core::Engine::Quit(bool RestartEngine)
{
	RunningApplicationInstance = false;
	SignalRestart = RestartEngine;
}

void Core::Engine::BeginApplicationMainLoop()
{
	ApplicationInstance->Begin();

	while (RunningApplicationInstance)
	{
		Platform::PerFrameUpdate();

		const double FrameDeltaSeconds = Platform::GetFrameMicroseconds() * 1e-6;

		//const double AverageFPS = 1.0 / FrameDeltaSeconds;
		//const double AverageMilliseconds = 1.0 / AverageFPS;
		//const uint32_t AverageFPSWhole = static_cast<uint32_t>(AverageFPS);

		FixedTickApplication(FrameDeltaSeconds);
		TickApplication(FrameDeltaSeconds);
	}

	ApplicationInstance->End();
}

void Core::Engine::FixedTickApplication(double FrameDeltaSeconds)
{
	FixedTimeAccumulationSeconds += FrameDeltaSeconds;

	while (FixedTimeAccumulationSeconds > TimeElapsedBetweenFixedTicksSeconds)
	{
		ApplicationInstance->FixedTick(FixedTimestep);
		FixedTimeAccumulationSeconds -= TimeElapsedBetweenFixedTicksSeconds;
	}
}

void Core::Engine::TickApplication(double FrameDeltaSeconds)
{
	ApplicationInstance->Tick(FrameDeltaSeconds);
}

bool Core::Engine::CallPlatformCreateWindowImplementation(Core::Window& Temp, const Core::WindowCreateParameters& Parameters)
{
	return Platform::CreatePlatformWindow(Temp, Parameters);
}
