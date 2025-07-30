#include "Window.h"

Core::Window::~Window()
{
	// TODO: Destroy the platform window with the platform implementation. Possibly pass an engine instance reference to the window when creating 
	// it so the window can access exposed platform implementation
}

void Core::Window::SetPlatformHandle(void* InHandle)
{
	PlatformHandle = InHandle;
}
