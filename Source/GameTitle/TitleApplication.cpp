#include "TitleApplication.h"
#include "Core/ConsoleOutput.h"
#include "Core/NotificationManager.h"

IMPLEMENT(TitleApplication)

TitleApplication::TitleApplication(Core::Engine& EngineInstanceRunningApplication)
	: Core::Application(EngineInstanceRunningApplication)
{
	GetEngine().CreateConsoleWindow();
	CONSOLE_PRINTF("hello title application\n");

	GetEngine().GetNotificationManager().AddNotificationListenerMethod<TitleApplication, &TitleApplication::NotificationListener>(this);
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
	}
}
