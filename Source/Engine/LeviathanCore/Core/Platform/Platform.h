#pragma once

namespace Core
{
	class NotificationManager;
	class Window;

	struct WindowCreateParameters;
	struct Rectangle;

	namespace Platform
	{
		bool Initialize(Core::NotificationManager* pNotificationManager);
		void PerFrameUpdate();
		double GetFrameMicroseconds();
		bool CreateConsole();
		bool RemoveConsole();

		// Creates a new platform window instance with the in parameters. Returns true if succesful otherwise, returns false
		bool CreatePlatformWindow(Core::Window& Temp, const Core::WindowCreateParameters& Parameters);

		bool DestroyPlatformWindow(Core::Window& WindowToDestroy);
		bool MakePlatformWindowFullscreen(Core::Window& WindowToMakeFullscreen);
		bool ExitPlatformWindowFullscreen(Core::Window& WindowToExitFullscreen);
		bool CaptureCursor(const Core::Rectangle& CaptureRegion);
		bool UncaptureCursor();
		Core::Rectangle GetPlatformWindowRegion(const Core::Window& TargetWindow);
		Core::Rectangle GetPlatformWindowClientRegion(const Core::Window& TargetWindow);
		bool SetCursorPosition(int32_t X, int32_t Y);
		bool SetCursorPositionRelativeToWindow(int32_t X, int32_t Y, const Core::Window& TargetWindow);
		bool IsPlatformWindowFocused(const Core::Window& TargetWindow);
		bool IsPlatformWindowMinimized(const Core::Window& TargetWindow);
	}
}