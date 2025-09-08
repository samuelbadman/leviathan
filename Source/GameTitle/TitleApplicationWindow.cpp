#include "TitleApplicationWindow.h"

TitleApplicationWindow::TitleApplicationWindow(Core::Engine& Engine, const char* InUniqueName)
	: Super(Engine, InUniqueName)
{
}

void TitleApplicationWindow::OnInputKey(const Core::InputEventArgs& EventArgs)
{

}

void TitleApplicationWindow::OnInputAxis(const Core::InputEventArgs& EventArgs)
{

}

void TitleApplicationWindow::OnResized(uint32_t NewWidth, uint32_t NewHeight)
{
	TitleApplicationWindowResizedParameters Params = {};
	Params.pWindow = this;
	Params.NewWidth = NewWidth;
	Params.NewHeight = NewHeight;

	ResizedDelegate.Execute(Params);
}
