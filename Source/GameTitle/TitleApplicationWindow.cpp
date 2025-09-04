#include "TitleApplicationWindow.h"
#include "Core/Engine.h"
#include "Core/ConsoleOutput.h"

TitleApplicationWindow::TitleApplicationWindow(Core::Engine& Engine, const char* InUniqueName)
	: Super(Engine, InUniqueName)
{
	CONSOLE_PRINTF("Hello title application window.\n");
}

void TitleApplicationWindow::OnInputKey(const Core::InputEventArgs& EventArgs)
{

}

void TitleApplicationWindow::OnInputAxis(const Core::InputEventArgs& EventArgs)
{

}

void TitleApplicationWindow::OnResized(uint32_t NewWidth, uint32_t NewHeight)
{
	TitleApplicationWindowResizedDelegateParameters Params = {};
	Params.pWindow = this;
	Params.NewWidth = NewWidth;
	Params.NewHeight = NewHeight;

	ResizedDelegate.Execute(Params);
}
