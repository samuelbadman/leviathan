#include "Window.h"
#include "Engine.h"

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

void Core::Window::SetFullscreenFlag(const bool NewFullscreen)
{
	FullscreenFlag = NewFullscreen;
}

void Core::Window::SetModeFlag(const Core::WindowMode NewMode)
{
	ModeFlag = NewMode;
}

void Core::Window::GetTopLeftCoordOnEnterFullscreen(int32_t& X, int32_t& Y) const
{
	X = TopLeftCoordOnEnterFullscreen[0];
	Y = TopLeftCoordOnEnterFullscreen[1];
}

void Core::Window::SetTopLeftCoordOnEnterFullscreen(const int32_t X, const int32_t Y)
{
	TopLeftCoordOnEnterFullscreen[0] = X;
	TopLeftCoordOnEnterFullscreen[1] = Y;
}

void Core::Window::GetDimensionsOnEnterFullscreen(int32_t& Width, int32_t& Height) const
{
	Width = DimensionsOnEnterFullscreen[0];
	Height = DimensionsOnEnterFullscreen[1];
}

void Core::Window::SetDimensionsOnEnterFullscreen(const int32_t Width, const int32_t Height)
{
	DimensionsOnEnterFullscreen[0] = Width;
	DimensionsOnEnterFullscreen[1] = Height;
}

void Core::Window::OnResized(uint32_t NewWidth, uint32_t NewHeight)
{
	WindowResizedDelegateParameters Params = {};
	Params.pWindow = this;
	Params.NewWidth = NewWidth;
	Params.NewHeight = NewHeight;

	ResizedDelegate.Execute(Params);
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
	DestroyedDelegate.Execute();
}

void Core::Window::Close()
{
	OnCloseSignal();
}

void Core::Window::MakeFullscreen()
{
	EngineInstance.MakeWindowFullscreenOnPlatform(*this);
}

void Core::Window::ExitFullscreen()
{
	EngineInstance.ExitWindowFullscreenOnPlatform(*this);
}

Core::Rectangle Core::Window::GetRegion() const
{
	return EngineInstance.GetWindowRegionOnPlatform(*this);
}

Core::Rectangle Core::Window::GetClientRegion() const
{
	return EngineInstance.GetWindowClientRegionOnPlatform(*this);
}

void Core::Window::CaptureCursorInWindowRegion() const
{
	EngineInstance.CaptureCursorOnPlatform(GetRegion());
}

void Core::Window::CaptureCursorInClientRegion() const
{
	EngineInstance.CaptureCursorOnPlatform(GetClientRegion());
}

bool Core::Window::IsFocused() const
{
	return EngineInstance.IsWindowFocusedOnPlatform(*this);
}

bool Core::Window::IsMinimized() const
{
	return EngineInstance.IsWindowMinimizedOnPlatform(*this);
}

bool Core::Window::ChangeMode(const Core::WindowMode NewMode)
{
	return EngineInstance.ChangeWindowModeOnPlatform(NewMode, *this);
}
