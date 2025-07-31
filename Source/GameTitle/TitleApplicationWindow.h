#pragma once

#include "Core/Window.h"

class TitleApplicationWindow : public Core::Window
{
public:
	TitleApplicationWindow(Core::Engine& Engine, const Core::WindowCreateParameters& InCreationParameters);

private:
	// Begin window interface
	virtual void OnCloseSignal() override;
	// End window interface
};