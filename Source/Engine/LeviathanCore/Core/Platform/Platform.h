#pragma once

namespace Core
{
	enum class WindowMode : uint8_t;

	class Engine;
	class Window;

	struct WindowCreateParameters;
	struct Rectangle;

	namespace Platform
	{
		bool Initialize(Core::Engine& EngineInstance);
		void UpdateMessageQueue();
		void Update();
		double GetUpdateMicroseconds();
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
		bool SetPlatformWindowMode(const Core::WindowMode NewMode, Core::Window& TargetWindow);
		void ShowMouseCursor(bool Show);
	}
}