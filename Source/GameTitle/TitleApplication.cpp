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

	// Add title application notification listener
	GetEngine().GetNotificationManager().AddNotificationListenerMethod<TitleApplication, &TitleApplication::NotificationListener>(this);

	// Create a main window for the title application
	InitializeMainAppWindow();

	// Initialize rendering for the application
	InitializeRendering();
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

	default: break;
	}
}

void TitleApplication::OnMainAppWindowDestroyed()
{
	// Delete app window rendering context
	if (RendererModuleInstance && MainAppWindow)
	{
		RendererModuleInstance->DeleteContext(MainAppWindow->GetPlatformHandle(), AppWindowRenderingContext);
	}

	// Tell the engine to quit
	GetEngine().Quit();
}

void TitleApplication::OnMainAppWindowResized(const TitleApplicationWindowResizedParameters& Params)
{
}

bool TitleApplication::InitializeMainAppWindow()
{
	Core::WindowCreateParameters AppWindowCreateParameters = {};
	AppWindowCreateParameters.UniqueWindowName = "MainTitleAppWindow";
	AppWindowCreateParameters.WindowTitle = "Main app window";
	AppWindowCreateParameters.Mode = Core::WindowMode::Windowed;
	AppWindowCreateParameters.Width = 960;
	AppWindowCreateParameters.Height = 540;

	MainAppWindow = GetEngine().CreateWindowOnPlatform<TitleApplicationWindow>(AppWindowCreateParameters);

	if (!MainAppWindow)
	{
		return false;
	}

	// Bind to main app window resized delegate
	MainAppWindow->GetResizedDelegate().BindMethod<TitleApplication, &TitleApplication::OnMainAppWindowResized>(this);

	// Bind to main app window destroyed delegate
	MainAppWindow->GetDestroyedDelegate().BindMethod<TitleApplication, &TitleApplication::OnMainAppWindowDestroyed>(this);

	return true;
}

bool TitleApplication::InitializeRendering()
{
	// Create rendering module
	RendererModuleInstance = GetEngine().CreateModule<Renderer::RendererModule>();

	if (!RendererModuleInstance)
	{
		return false;
	}

	// Create a rendering context for the main title application window
	AppWindowRenderingContext = RendererModuleInstance->CreateContext(MainAppWindow->GetPlatformHandle());
	if (!AppWindowRenderingContext)
	{
		return false;
	}

	// Make the main title application window rendering context the current rendering context
	if (!RendererModuleInstance->MakeContextCurrent(MainAppWindow->GetPlatformHandle(), AppWindowRenderingContext))
	{
		return false;
	}

	// Load the renderer api functions. This only needs to be done once during startup but needs a valid rendering context to be made current
	if (!RendererModuleInstance->LoadAPI())
	{
		return false;
	}

	// Debug print renderer API version
	RendererModuleInstance->PrintVersion();

	return true;
}
