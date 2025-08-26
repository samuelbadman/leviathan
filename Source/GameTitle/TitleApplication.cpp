#include "TitleApplication.h"
#include "Core/ConsoleOutput.h"
#include "Core/NotificationManager.h"
#include "TitleApplicationWindow.h"
#include "Renderer/RendererModule.h"

IMPLEMENT(TitleApplication)

TitleApplication::TitleApplication(Core::Engine& EngineInstanceRunningApplication)
	: Super(EngineInstanceRunningApplication)
{
	// Create console output window
	GetEngine().CreateConsoleWindowOnPlatform();
	CONSOLE_PRINTF("Hello title application.\n");

	// Add title application notification listener
	GetEngine().GetNotificationManager().AddNotificationListenerMethod<TitleApplication, &TitleApplication::NotificationListener>(this);

	// Create engine modules used by title application
	RendererModuleInstance = GetEngine().CreateModule<Renderer::RendererModule>();

	// Create a window for the title application
	Core::WindowCreateParameters AppWindowCreateParameters = {};
	AppWindowCreateParameters.UniqueWindowName = "TitleAppWindow";
	AppWindowCreateParameters.WindowName = "Title application";
	AppWindowCreateParameters.Mode = Core::WindowMode::Windowed;
	AppWindowCreateParameters.Width = 640 * 2;
	AppWindowCreateParameters.Height = 360 * 2;

	AppWindow = GetEngine().CreateWindowOnPlatform<TitleApplicationWindow>(AppWindowCreateParameters);
}

TitleApplication::~TitleApplication()
{
	// Remove console output window
	GetEngine().RemoveConsoleWindowOnPlatform();

	// Remove title application notification listener
	GetEngine().GetNotificationManager().RemoveNotificationListenerMethod<TitleApplication, &TitleApplication::NotificationListener>(this);
}

void TitleApplication::NotificationListener(const Core::NotificationData& Notification)
{
	switch (Notification.Type)
	{
	case Core::NotificationType::GamepadConnected:
		CONSOLE_PRINTF("Gamepad connected at connection %d\n", Notification.Payload.GamepadConnectedPayload.ConnectionIndex);
		break;

	case Core::NotificationType::GamepadDisconnected:
		CONSOLE_PRINTF("Gamepad disconnected at connection %d\n", Notification.Payload.GamepadDisconnectedPayload.ConnectionIndex);
		break;

	case Core::NotificationType::WindowDestroyed:
		if (Notification.Payload.WindowDestroyedPayload.DestroyedWindow == AppWindow.get())
		{
			CONSOLE_PRINTF("Title application window has been destroyed\n");
			GetEngine().Quit();
		}
		break;

	default: break;
	}
}
