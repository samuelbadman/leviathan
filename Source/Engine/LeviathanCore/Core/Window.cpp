#include "Window.h"
#include "Engine.h"
#include "NotificationManager.h"

Core::Window::Window(Core::Engine& Engine, const char* InUniqueName)
	: EngineInstance(Engine), UniqueName(InUniqueName)
{
}

Core::Window::~Window()
{
	if (PlatformHandle)
	{
		EngineInstance.DestroyWindowOnPlatform(*this);
	}
}

void Core::Window::SetPlatformHandle(void* InHandle)
{
	PlatformHandle = InHandle;
}

void Core::Window::OnForceClose()
{
	Close();
}

void Core::Window::OnCloseSignal()
{
	EngineInstance.DestroyWindowOnPlatform(*this);
}

void Core::Window::OnDestroyed()
{
	Core::NotificationData WindowDestroyedNotificationData = {};
	WindowDestroyedNotificationData.Type = Core::NotificationType::WindowDestroyed;
	WindowDestroyedNotificationData.Payload.WindowDestroyedPayload = {};
	WindowDestroyedNotificationData.Payload.WindowDestroyedPayload.DestroyedWindow = this;

	EngineInstance.GetNotificationManager().SendNotification(WindowDestroyedNotificationData);
}

void Core::Window::Close()
{
	OnCloseSignal();
}
