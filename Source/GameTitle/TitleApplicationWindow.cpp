#include "TitleApplicationWindow.h"
#include "Core/ConsoleOutput.h"	
#include "Core/Engine.h"

TitleApplicationWindow::TitleApplicationWindow(Core::Engine& Engine, const char* InUniqueName)
	: Super(Engine, InUniqueName)
{
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
