#include "TitleApplicationWindow.h"
#include "Core/ConsoleOutput.h"	
#include "Core/Engine.h"

TitleApplicationWindow::TitleApplicationWindow(Core::Engine& Engine)
	: Core::Window(Engine)
{
}

void TitleApplicationWindow::OnCloseSignal()
{
	CONSOLE_PRINTF("title application window received close signal\n");

	Super::OnCloseSignal();
}
