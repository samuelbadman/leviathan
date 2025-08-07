#pragma once

namespace Core
{
	class NotificationManager;
	class Window;

	struct WindowCreateParameters;

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
	}
}