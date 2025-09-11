#include "TitleApplication.h"
#include "Core/ConsoleOutput.h"
#include "Core/NotificationManager.h"
#include "TitleApplicationWindow.h"
#include "Rendering/RenderingModule.h"

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

void TitleApplication::Tick(double DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	// Render
	RenderApp();
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
	// Shutdown rendering for the application
	ShutdownRendering();

	// Tell the engine to quit
	GetEngine().Quit();
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

	// Bind to main app window destroyed delegate
	MainAppWindow->GetDestroyedDelegate().AddMethod<TitleApplication, &TitleApplication::OnMainAppWindowDestroyed>(this);

	return true;
}

bool TitleApplication::InitializeRendering()
{
	// Create rendering module
	RenderingModuleInstance = GetEngine().CreateModule<Rendering::RenderingModule>();

	if (!RenderingModuleInstance || !MainAppWindow)
	{
		return false;
	}

	// Initialize rendering module
	if (!RenderingModuleInstance->Initialize(MainAppWindow->GetPlatformHandle()))
	{
		return false;
	}

	// Set main app window as the current rendering context
	RenderingModuleInstance->SetRenderOutputWindow(MainAppWindow.get());


	// Test setup rendering scene
	const std::array<float, 3 * 3> Vertices =
	{
		// Bottom left
		-0.5f, -0.5f, 0.0f,
		// Bottom right
		0.5f, -0.5f, 0.0f,
		// Top
		0.0f,  0.5f, 0.0f
	};

	TriangleRenderMeshID = RenderingModuleInstance->CreateRenderMeshObject(sizeof(Vertices), Vertices.data());

	return true;
}

bool TitleApplication::ShutdownRendering()
{
	// Test destroy rendering scene
	if (RenderingModuleInstance)
	{
		RenderingModuleInstance->DestroyRenderMeshObject(TriangleRenderMeshID);
	}

	// Delete app window rendering context
	if (RenderingModuleInstance && MainAppWindow)
	{
		RenderingModuleInstance->SetRenderOutputWindow(nullptr);
		return RenderingModuleInstance->DestroyRenderOutputWindowResources(MainAppWindow->GetPlatformHandle());
	}
	return false;
}

void TitleApplication::RenderApp()
{
	if (RenderingModuleInstance)
	{
		RenderingModuleInstance->Render();
	}
}
