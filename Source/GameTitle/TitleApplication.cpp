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

	// Create a main window for the title application
	Core::WindowCreateParameters AppWindowCreateParameters = {};
	AppWindowCreateParameters.UniqueWindowName = "TitleAppWindow";
	AppWindowCreateParameters.WindowName = "Title application";
	AppWindowCreateParameters.Mode = Core::WindowMode::Windowed;
	AppWindowCreateParameters.Width = 960;
	AppWindowCreateParameters.Height = 540;

	AppWindow = GetEngine().CreateWindowOnPlatform<TitleApplicationWindow>(AppWindowCreateParameters);

	if (AppWindow)
	{
		// Subscribe to app window resized delegate
		AppWindow->GetResizedDelegate().BindMethod<TitleApplication, &TitleApplication::OnAppWindowResized>(this);

		if (RendererModuleInstance)
		{
			// Create a rendering context for the title application window
			AppWindowRenderingContext = RendererModuleInstance->CreateContext(AppWindow->GetPlatformHandle());
			if (!AppWindowRenderingContext)
			{
				CONSOLE_PRINTF("Failed to create rendering context for title application window.\n");
			}

			// Make the title application window rendering context the current rendering context
			if (!RendererModuleInstance->MakeContextCurrent(AppWindow->GetPlatformHandle(), AppWindowRenderingContext))
			{
				CONSOLE_PRINTF("Failed to make title application window rendering context current.\n");
			}

			// Load the renderer api functions. This only needs to be done once during startup but needs a valid rendering context to be made current
			if (!RendererModuleInstance->LoadAPI())
			{
				CONSOLE_PRINTF("Failed to load rendering API functions.\n");
			}

			RendererModuleInstance->PrintVersion();

			RendererModuleInstance->Viewport(0, 0, AppWindowCreateParameters.Width, AppWindowCreateParameters.Height);
		}
	}
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
			OnAppWindowDestroyed();
		}
		break;

	default: break;
	}
}

void TitleApplication::OnAppWindowDestroyed()
{
	// Delete app window rendering context
	if (!RendererModuleInstance->DeleteContext(AppWindow->GetPlatformHandle(), AppWindowRenderingContext))
	{
		CONSOLE_PRINTF("Failed to delete title application window's rendering context.\n");
	}

	// Tell the engine to quit
	GetEngine().Quit();
}

void TitleApplication::OnAppWindowResized(const TitleApplicationWindowResizedDelegateParameters& Params)
{
	CONSOLE_PRINTF("Title application window resized. NewWidth: %d, NewHeight: %d.\n", Params.NewWidth, Params.NewHeight);
	if (RendererModuleInstance)
	{
		RendererModuleInstance->Viewport(0, 0, Params.NewWidth, Params.NewHeight);
	}
}
