#pragma once

#include "Core/Window.h"

class TitleApplicationWindow : public Core::Window
{
public:
	TitleApplicationWindow(Core::Engine& Engine, const char* InUniqueName);

private:
	// Begin window interface
	virtual void OnInputKey(const Core::InputEventArgs& EventArgs) override;
	virtual void OnInputAxis(const Core::InputEventArgs& EventArgs) override;
	virtual void OnResized(uint32_t NewWidth, uint32_t NewHeight) override;
	// End window interface
};