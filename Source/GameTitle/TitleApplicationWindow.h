#pragma once

#include "Window.h"

class TitleApplicationWindow : public Core::Window
{
public:
	TitleApplicationWindow(Core::Engine& Engine, const char* InUniqueName);

private:
	// Begin window interface
	virtual void OnInputKey(const Core::InputEventArgs& EventArgs) override;
	virtual void OnInputAxis(const Core::InputEventArgs& EventArgs) override;
	// End window interface
};