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

		// Creates a new platform window instance with the in parameters. Returns a pointer to the created window instance if succesful otherwise, returns null
		std::unique_ptr<Core::Window> CreatePlatformWindow(const Core::WindowCreateParameters& Parameters);
	}
}