#include "TitleApplicationWindow.h"
#include "Core/ConsoleOutput.h"	
#include "Core/Engine.h"

TitleApplicationWindow::TitleApplicationWindow(Core::Engine& Engine, const char* InUniqueName)
	: Super(Engine, InUniqueName)
{
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
