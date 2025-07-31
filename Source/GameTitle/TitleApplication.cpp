#include "TitleApplication.h"
#include "Core/ConsoleOutput.h"
#include "Core/NotificationManager.h"
#include "TitleApplicationWindow.h"

IMPLEMENT(TitleApplication)

TitleApplication::TitleApplication(Core::Engine& EngineInstanceRunningApplication)
	: Super(EngineInstanceRunningApplication)
{
	// Create console output window
	GetEngine().CreateConsoleWindow();
	CONSOLE_PRINTF("hello title application\n");

	// Add title application notification listener
	GetEngine().GetNotificationManager().AddNotificationListenerMethod<TitleApplication, &TitleApplication::NotificationListener>(this);

	// Create a window for the title application
	Core::WindowCreateParameters AppWindowCreateParameters = {};
	AppWindowCreateParameters.UniqueWindowName = "AppWindow";
	AppWindowCreateParameters.WindowName = "Title application";
	AppWindowCreateParameters.Mode = Core::WindowMode::Windowed;
	AppWindowCreateParameters.Width = 1280;
	AppWindowCreateParameters.Height = 720;

	AppWindow = GetEngine().CreateWindowOnPlatform<TitleApplicationWindow>(AppWindowCreateParameters);

	if (AppWindow)
	{
		CONSOLE_PRINTF("successfully created title application window\n");
	}
}

TitleApplication::~TitleApplication()
{
	// Remove console output window
	GetEngine().RemoveConsoleWindow();

	// Remove title application notification listener
	GetEngine().GetNotificationManager().RemoveNotificationListenerMethod<TitleApplication, &TitleApplication::NotificationListener>(this);
}

void TitleApplication::NotificationListener(const Core::NotificationData& Notification)
{
	switch (Notification.Type)
	{
	case Core::NotificationType::GameControllerConnected:
		CONSOLE_PRINTF("game controller connected notification\n");
		break;

	case Core::NotificationType::GameControllerDisconnected:
		CONSOLE_PRINTF("game controller disconnected notification\n");
		break;

	case Core::NotificationType::WindowDestroyed:
		if (Notification.Payload.WindowDestroyedPayload.DestroyedWindow == AppWindow.get())
		{
			CONSOLE_PRINTF("title application window has been destroyed\n");
			GetEngine().Quit();
		}
		break;

	default: break;
	}
}
