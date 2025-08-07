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

bool Core::Engine::CreateConsoleWindowOnPlatform() const
{
	return Platform::CreateConsole();
}

bool Core::Engine::RemoveConsoleWindowOnPlatform() const
{
	return Platform::RemoveConsole();
}

bool Core::Engine::DestroyWindowOnPlatform(Core::Window& WindowToDestroy) const
{
	if (Platform::DestroyPlatformWindow(WindowToDestroy))
	{
		WindowToDestroy.SetPlatformHandle(nullptr);
		return true;
	}
	return false;
}

bool Core::Engine::MakeWindowFullscreenOnPlatform(Core::Window& WindowToMakeFullscreen) const
{
	if (WindowToMakeFullscreen.IsFullscreen())
	{
		return false;
	}

	const Core::Rectangle RegionBeforeFullscreen = GetWindowRegionOnPlatform(WindowToMakeFullscreen);

	WindowToMakeFullscreen.SetTopLeftCoordOnEnterFullscreen(static_cast<int32_t>(RegionBeforeFullscreen.Left), static_cast<int32_t>(RegionBeforeFullscreen.Top));
	WindowToMakeFullscreen.SetDimensionsOnEnterFullscreen(static_cast<int32_t>(RegionBeforeFullscreen.Right - RegionBeforeFullscreen.Left),
		static_cast<int32_t>(RegionBeforeFullscreen.Bottom - RegionBeforeFullscreen.Top));

	if (!Platform::MakePlatformWindowFullscreen(WindowToMakeFullscreen))
	{
		return false;
	}

	WindowToMakeFullscreen.SetFullscreenFlag(true);
	WindowToMakeFullscreen.OnEnterFullscreen();
	return true;
}

bool Core::Engine::ExitWindowFullscreenOnPlatform(Core::Window& WindowToExitFullscreen) const
{
	if (!WindowToExitFullscreen.IsFullscreen())
	{
		return false;
	}

	if (!Platform::ExitPlatformWindowFullscreen(WindowToExitFullscreen))
	{
		return false;
	}

	WindowToExitFullscreen.SetFullscreenFlag(false);
	WindowToExitFullscreen.OnExitFullscreen();
	return true;
}

bool Core::Engine::CaptureCursorOnPlatform(Core::Rectangle& CaptureRegion) const
{
	return Platform::CaptureCursor(CaptureRegion);
}

bool Core::Engine::UncaptureCursorOnPlatform() const
{
	return Platform::UncaptureCursor();
}

Core::Rectangle Core::Engine::GetWindowRegionOnPlatform(const Core::Window& TargetWindow) const
{
	return Platform::GetPlatformWindowRegion(TargetWindow);
}

Core::Rectangle Core::Engine::GetWindowClientRegionOnPlatform(const Core::Window& TargetWindow) const
{
	return Platform::GetPlatformWindowClientRegion(TargetWindow);
}

bool Core::Engine::SetCursorPositionOnPlatform(int32_t X, int32_t Y) const
{
	return Platform::SetCursorPosition(X, Y);
}

bool Core::Engine::SetCursorPositionRelativeToWindowOnPlatform(int32_t X, int32_t Y, const Core::Window& RelativeWindow) const
{
	return Platform::SetCursorPositionRelativeToWindow(X, Y, RelativeWindow);
}

bool Core::Engine::IsWindowFocusedOnPlatform(const Core::Window& TargetWindow)
{
	return Platform::IsPlatformWindowFocused(TargetWindow);
}

bool Core::Engine::IsWindowMinimizedOnPlatform(const Core::Window& TargetWindow) const
{
	return Platform::IsPlatformWindowMinimized(TargetWindow);
}

bool Core::Engine::ChangeWindowModeOnPlatform(const Core::WindowMode NewMode, Core::Window& TargetWindow) const
{
	if (NewMode == Core::WindowMode::MAX || TargetWindow.GetModeFlag() == NewMode || TargetWindow.IsFullscreen())
	{
		return false;
	}

	if (Platform::SetPlatformWindowMode(NewMode, TargetWindow))
	{
		TargetWindow.SetModeFlag(NewMode);
		return true;
	}

	return false;
}

Core::NotificationManager& Core::Engine::GetNotificationManager() const
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

bool Core::Engine::CallPlatformCreateWindowImplementation(Core::Window& Temp, const Core::WindowCreateParameters& Parameters) const
{
	return Platform::CreatePlatformWindow(Temp, Parameters);
}
