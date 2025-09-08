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

void TitleApplicationWindow::OnResized(uint32_t NewWidth, uint32_t NewHeight)
{
	Super::OnResized(NewWidth, NewHeight);

	TitleApplicationWindowResizedParameters Params = {};
	Params.pWindow = this;
	Params.NewWidth = NewWidth;
	Params.NewHeight = NewHeight;

	ResizedDelegate.Execute(Params);
}

void TitleApplicationWindow::OnDestroyed()
{
	Super::OnDestroyed();

	DestroyedDelegate.Execute();
}
