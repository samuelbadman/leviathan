#include "TitleApplicationWindow.h"
#include "Core/ConsoleOutput.h"	
#include "Core/Engine.h"

TitleApplicationWindow::TitleApplicationWindow(Core::Engine& Engine, const char* InUniqueName)
	: Super(Engine, InUniqueName)
{
}

void TitleApplicationWindow::OnInputKey(const Core::InputEventArgs& EventArgs)
{
	if (EventArgs.Key == Core::Keys::F && EventArgs.Event == Core::InputEvent::Pressed)
	{
		if (IsFullscreen())
		{
			ExitFullscreen();
		}
		else
		{
			MakeFullscreen();
		}
	}
	else if (EventArgs.Key == Core::Keys::C && EventArgs.Event == Core::InputEvent::Pressed)
	{
		static bool IsCursorCaptured = false;

		if (IsCursorCaptured)
		{
			GetEngine().UncaptureCursorOnPlatform();
			IsCursorCaptured = false;
		}
		else
		{
			CaptureCursorInClientRegion();
			IsCursorCaptured = true;
		}
	}
	else if (EventArgs.Key == Core::Keys::S && EventArgs.Event == Core::InputEvent::Pressed)
	{
		GetEngine().SetCursorPositionRelativeToWindowOnPlatform(0, 0, *this);
	}
	else if (EventArgs.Key == Core::Keys::M && EventArgs.Event == Core::InputEvent::Pressed)
	{
		ChangeMode(Core::WindowMode::Borderless);
	}

	std::string EventString;
	switch (EventArgs.Event)
	{
	case Core::InputEvent::Pressed: EventString = "Pressed"; break;
	case Core::InputEvent::Repeat: EventString = "Repeat"; break;
	case Core::InputEvent::Released: EventString = "Released"; break;
	default: EventString = "None"; break;
	}

	CONSOLE_PRINTF("title application window input key event. Key: %s, Event: %s, Data: %f, Scan code: %d\n", EventArgs.Key.ToString().c_str(), 
		EventString.c_str(), EventArgs.Data, EventArgs.Key.GetScanCode());
}

void TitleApplicationWindow::OnInputAxis(const Core::InputEventArgs& EventArgs)
{
	std::string EventString;
	switch (EventArgs.Event)
	{
	case Core::InputEvent::Pressed: EventString = "Pressed"; break;
	case Core::InputEvent::Repeat: EventString = "Repeat"; break;
	case Core::InputEvent::Released: EventString = "Released"; break;
	default: EventString = "None"; break;
	}

	CONSOLE_PRINTF("title application window input axis event. Key: %s, Event: %s, Data: %f, Scan code: %d\n", EventArgs.Key.ToString().c_str(),
		EventString.c_str(), EventArgs.Data, EventArgs.Key.GetScanCode());
}

void TitleApplicationWindow::OnMaximized()
{
	CONSOLE_PRINTF("title application window maximized\n");
}

void TitleApplicationWindow::OnMinimized()
{
	CONSOLE_PRINTF("title application window minimized\n");
}

void TitleApplicationWindow::OnResized(uint32_t NewWidth, uint32_t NewHeight)
{
	CONSOLE_PRINTF("title application window resized. New width: %d, New height: %d\n", NewWidth, NewHeight);
}

void TitleApplicationWindow::OnEnterFullscreen()
{
	CONSOLE_PRINTF("title application window entered fullscreen\n");
}

void TitleApplicationWindow::OnExitFullscreen()
{
	CONSOLE_PRINTF("title application window exited fullscreen\n");
}

void TitleApplicationWindow::OnEnterSizeMove()
{
	CONSOLE_PRINTF("title application window enter size move\n");
}

void TitleApplicationWindow::OnExitSizeMove()
{
	CONSOLE_PRINTF("title application window exit size move\n");
}

void TitleApplicationWindow::OnReceivedFocus()
{
	CONSOLE_PRINTF("title application window received focus\n");
}

void TitleApplicationWindow::OnLostFocus()
{
	CONSOLE_PRINTF("title application window lost focus\n");
}

void TitleApplicationWindow::OnCloseSignal()
{
	CONSOLE_PRINTF("title application window received close signal\n");

	Super::OnCloseSignal();
}
