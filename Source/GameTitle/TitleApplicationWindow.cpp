#include "TitleApplicationWindow.h"

TitleApplicationWindow::TitleApplicationWindow(Core::Engine& Engine, const char* InUniqueName)
	: Super(Engine, InUniqueName)
{
}

void TitleApplicationWindow::OnInputKey(const Core::InputEventArgs& EventArgs)
{
	Super::OnInputKey(EventArgs);
}

void TitleApplicationWindow::OnInputAxis(const Core::InputEventArgs& EventArgs)
{
	Super::OnInputAxis(EventArgs);
}
