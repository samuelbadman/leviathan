#include "RenderingModule.h"
#include "RenderHardwareInterface/RenderHardwareInterface.h"
#include "Core/Window.h"

Rendering::RenderingModule::~RenderingModule()
{
	RenderHardwareInterface::Cleanup();
}

bool Rendering::RenderingModule::Initialize(void* const OutputWindowPlatformHandle)
{
	return RenderHardwareInterface::Initialize(OutputWindowPlatformHandle);
}

bool Rendering::RenderingModule::CreateRenderOutputWindowResources(void* const OutputWindowPlatformHandle)
{
	return RenderHardwareInterface::CreateOutputWindowResources(OutputWindowPlatformHandle);
}

bool Rendering::RenderingModule::DestroyRenderOutputWindowResources(void* const OutputWindowPlatformHandle)
{
	return RenderHardwareInterface::DestroyOutputWindowResources(OutputWindowPlatformHandle);
}

void Rendering::RenderingModule::SetRenderOutputWindow(Core::Window* OutputWindow)
{
	// If a render output window is already set remove the rendering module's binding to its resized event delegate and clear the render output window pointer
	if (CurrentRenderOutputWindow)
	{
		CurrentRenderOutputWindow->GetResizedDelegate().RemoveMethod<Rendering::RenderingModule, &Rendering::RenderingModule::OnRenderOutputWindowResized>(this);

		CurrentRenderOutputWindow = nullptr;
	}

	// Try to set the new render output window
	if (OutputWindow)
	{
		if (RenderHardwareInterface::SetOutputWindow(OutputWindow->GetPlatformHandle()))
		{
			OutputWindow->GetResizedDelegate().AddMethod<Rendering::RenderingModule, &Rendering::RenderingModule::OnRenderOutputWindowResized>(this);

			CurrentRenderOutputWindow = OutputWindow;
		}
	}
}

void Rendering::RenderingModule::Render()
{
	if (CurrentRenderOutputWindow)
	{
		const Core::Rectangle OutputWindowClientRegionRect = CurrentRenderOutputWindow->GetClientRegion();
		RenderHardwareInterface::SetViewport(0,
			0,
			OutputWindowClientRegionRect.Right - OutputWindowClientRegionRect.Left,
			OutputWindowClientRegionRect.Bottom - OutputWindowClientRegionRect.Top);

		RenderHardwareInterface::ClearColorBuffer(0.2f, 0.3f, 0.3f, 1.0f);

		RenderHardwareInterface::SwapBuffers(CurrentRenderOutputWindow->GetPlatformHandle());
	}
}

void Rendering::RenderingModule::OnRenderOutputWindowResized(const Core::WindowResizedDelegateParameters& Parameters)
{
	Render();
}
